#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
static uintptr_t loader(PCB *pcb, const char *filename)
{
  // TODO();
  Elf_Ehdr ehdr;

  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // printf("Read 1\n");
  Elf_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(phdr, ehdr.e_ehsize, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; ++i)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0,
             phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
  // return 0;
}


// #define min(x,y) x < y ? x : y

// size_t ramdisk_read(void *buf, size_t offset, size_t len);
// size_t ramdisk_write(const void *buf, size_t offset, size_t len);
// size_t get_ramdisk_size();

// static uintptr_t loader(PCB *pcb, const char *filename) {
//   //TODO();
// 	Elf32_Ehdr elf_header;
// 	int success = ramdisk_read(&elf_header, 0, sizeof(Elf32_Ehdr));
// 	assert(success);
// 	assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
// 	Elf32_Off phoff = elf_header.e_phoff;
// 	for (int i = 0; i < elf_header.e_phnum; i ++) {
// 		Elf32_Phdr p_header;
// 		success = ramdisk_read(&p_header, phoff + i * sizeof(Elf32_Phdr), sizeof(Elf32_Phdr));
// 		assert(success);
// 		if (p_header.p_type == PT_LOAD) {
// 			Elf32_Addr virtAddr = p_header.p_vaddr;
// 			Elf32_Off segOffset = p_header.p_offset;
// 			Elf32_Word fileSz = p_header.p_filesz;
// 			Elf32_Word memSz = p_header.p_memsz;
			
// 			Elf32_Addr memPtr = virtAddr;
// 			Elf32_Off filePtr = segOffset;
// 			while (memPtr < virtAddr + fileSz) {
// 				int readSize = min(sizeof(size_t), virtAddr + fileSz - memPtr);
// 				if (readSize == sizeof(size_t)) {
// 					size_t readData;
// 					success = ramdisk_read(&readData, filePtr, readSize);
// 					assert(success);
// 					memcpy((void*)memPtr, &readData, readSize);
// 					memPtr += readSize;
// 					filePtr += readSize;
// 				}
// 				else {
// 					for(int bt = 0; bt < readSize; bt ++) {
// 						char readData;
// 						success = ramdisk_read(&readData, filePtr, 1);
// 						assert(success);
// 						memcpy((void*)memPtr, &readData, 1);
// 						filePtr ++;
// 						memPtr ++;
// 					}
// 				}
// 	//			printf("%d\n", virtAddr + fileSz - memPtr);
// 			}
// 			memPtr = virtAddr + fileSz;
// 			memset((void*)memPtr, 0, memSz - fileSz);

// 		}
// 	}
//   return elf_header.e_entry;
// }

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
