/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,
  TK_16,
  DEREF,
  TK_AND,
  TK_NEG,
  TK_REG

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},
    {"\\+", '+'},
    {"==", TK_EQ},
    {"-", '-'},
    {"\\*", '*'},
    {"/", '/'}, //- is not a metacharacter
    {"\\(", '('},
    {"\\)", ')'},
    {"[0-9]+", TK_NUM},
    {"^0x[0-9]+", TK_16},
    {"\\$[\\$a-z][0-9]+", TK_REG}, //[\S]表示，非空白就匹配

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[320] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        // Identify str as token，save in pmatch
        char *substr_start = e + position;
        // The starting string of token address is stored insubstr_start
        int substr_len = pmatch.rm_eo;
        // save token_len in substr_len

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        // if (substr_len > 30)
        //  substr_len = 30;
        // I do something wrong here...
        // perhaps i should not do something before TODO...

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        case TK_EQ:
          break;
        case TK_REG:
        if (substr_len > 31)
            substr_len = 31;
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
          nr_token++;
          // avoid overflow
          tokens[nr_token].str[substr_len] = '\0';
          break;
        case TK_16:
        case TK_NUM:
          if (substr_len > 31)
            substr_len = 31;
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          nr_token++;
          // avoid overflow
          tokens[nr_token].str[substr_len] = '\0';
          break;
        default:
          tokens[nr_token].type = rules[i].token_type;
          nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q)
{
  if (tokens[p].type != '(' || tokens[q].type != ')')

    return false;

  bool result = false;
  int count = 0;

  for (int i = p; i < q; i++)
  {
    if (tokens[i].type == '(')
    {
      count++;
    }
    else if (tokens[i].type == ')')
    {
      count--;
    }
    if (count == 0)
    {
      result = true;
    }
  }
  // printf("%d", count);
  if (count != 1)
  {
    panic("Error expression for () ");
  }
  if (result == true)
    return false;
  else
    return true;
}

word_t eval(int p, int q, bool *success)
{
  // success change always could be found
  if (p > q)
  {
    *success = false;
    return 0;
  }
  else if (p == q)
  {
    if (tokens[p].type == TK_NUM)
      return atoi(tokens[p].str);
    int x;
    sscanf(tokens[p].str, "%x", &x);
    return x;
  }
  else if (check_parentheses(p, q))
    // Are you write a python?
    // (4 + 3) * (2 - 1) just let it go...
    return eval(p + 1, q - 1, success);
  else if (tokens[p].type == TK_NEG && p == q + 1)
  {
    return -1 * atoi(tokens[q].str);
  }

  else
  {
    int op=0;
    int p_1 = 0;
    int te = -1;
    for (int i = p; i <= q; i++)
    {
      if (tokens[i].type == '(')
        p_1++;
      else if (tokens[i].type == ')')
      {
        p_1--;
      }
      else if (p_1 != 0)
        continue;
      // else if (te == -1 && (tokens[i].type == TK_NEG))
      // op = i;
      else if (te <= 1 && (tokens[i].type == '+' || tokens[i].type == '-'))
      {
        op = i;
        te = 1;
      }
      else if (te <= 0 && (tokens[i].type == '*' || tokens[i].type == '/'))
      {
        op = i;
        te = 0;
      }
    }
    int te1 = 1;
    int val1 = 0;
    if (tokens[op].type == TK_NEG && (tokens[p - 1].type == '+' || tokens[p - 1].type == '-' || tokens[p - 1].type == '*' || tokens[p - 1].type == '/'))
      te1 = 0;
    if (te1 == 1)
      val1 = eval(p, op - 1, success);
    int val2 = eval(op + 1, q, success);
    switch (tokens[op].type)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      if (val2 == 0)
      {
        printf("Zero is a divisor");
        *success = false;
        return 0;
      }
      return val1 / val2;
    default:
      printf("Unknown operation");
      *success = false;
    }
  }
  return 0;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  /* TODO: Implement code to evaluate the expression. */

  for (int i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '('))
      tokens[i].type = DEREF;
    else if (tokens[i].type == '-' && tokens[i - 1].type != TK_NUM && tokens[i - 1].type != ')')
    {
      tokens[i].type = TK_NEG;
    }

    else if (tokens[i].type == TK_REG)
    {
      bool success_reg = true;
      int val = isa_reg_str2val(tokens[i].str, &success_reg);
      if (success_reg == false)
        panic("not find this reg\n");
      tokens[i].type = TK_NUM;
      sprintf(tokens[i].str, "%x", val);
    }
  }
  *success = true;
  nr_token--;
  return eval(0, nr_token, success);
}
