#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/MpService.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#define LOCAL_APIC_REG  	0xFEE00000
#define INIT_IPI	 	0x000C4500
#define SIPI	 		0x000C4601
#define APIC_ID 		0xFEE00020

#define Assembler_Code_offset 0x1000
#define INIT_b 0b101
#define SIPI_b 0b110

volatile UINT32 * const APIC_ICR_LOW = (void *) 0xfee00300;
volatile UINT32 * const APIC_ICR_HIG = (void *) 0xfee00310;

typedef struct {
    EFI_MEMORY_DESCRIPTOR* MemoryMap;
    UINTN MemoryMapSize;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} memory_map_t;

typedef struct {
    UINTN ProcNum;
    UINTN ProcAllowed;
    EFI_PROCESSOR_INFORMATION Info;
} proc_info;

VOID EFIAPI asm_code_move(){
    unsigned char kursach_bin[] = {
            0xeb, 0x00, 0xfa, 0x0f, 0x01, 0x16, 0xc8, 0x10, 0x0f, 0x20, 0xc0, 0x0c,
            0x01, 0x0f, 0x22, 0xc0, 0xea, 0x15, 0x10, 0x08, 0x00, 0x66, 0xb8, 0x10,
            0x00, 0x8e, 0xc0, 0x8e, 0xd8, 0x26, 0xc7, 0x05, 0x00, 0x20, 0x20, 0x00,
            0x83, 0x00, 0x00, 0x00, 0x26, 0xc7, 0x05, 0x04, 0x20, 0x20, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x26, 0xc7, 0x05, 0x00, 0x10, 0x20, 0x00, 0x03, 0x20,
            0x20, 0x00, 0x26, 0xc7, 0x05, 0x04, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x26, 0xc7, 0x05, 0x00, 0x00, 0x20, 0x00, 0x03, 0x10, 0x20, 0x00,
            0x26, 0xc7, 0x05, 0x04, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8,
            0x00, 0x00, 0x20, 0x00, 0x0f, 0x22, 0xd8, 0xb9, 0x80, 0x00, 0x00, 0xc0,
            0x0f, 0x32, 0x0f, 0xba, 0xe8, 0x08, 0x0f, 0x30, 0x0f, 0x20, 0xc0, 0x0f,
            0xba, 0xe8, 0x1f, 0x0f, 0x22, 0xc0, 0xeb, 0x00, 0x48, 0xbf, 0x00, 0x00,
            0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc1, 0x80, 0x38, 0x01,
            0x00, 0x66, 0xc7, 0x07, 0x44, 0xe1, 0x48, 0xff, 0xc7, 0x48, 0xff, 0xc7,
            0xe2, 0xf3, 0xeb, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00, 0xff, 0xff, 0x00, 0x00,
            0x00, 0x92, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x20, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00, 0x27, 0x00, 0xa0, 0x10,
            0x00, 0x00

    };
    unsigned int kursach_bin_len = 206;

    volatile char *mem = (volatile char*)0x1000;
    for (UINTN i=0;i < kursach_bin_len;++i)
    {
        mem[i] = kursach_bin[i];
    }

}

VOID EFIAPI myPrint(EFI_PHYSICAL_ADDRESS lfb_base_addr, UINT32 color){
    UINT32 *b8 = (UINT32*)lfb_base_addr;
    for(UINTN i = 0; i < 80000; ++i)
        b8[i+80000] = color;
}

VOID EFIAPI INIT_SIPI_SIPI(UINT32     type,
        UINT32     init_code_entry,
        UINT32     ap_apic_id){
    *APIC_ICR_HIG = ((UINT32) ap_apic_id << 24);
    if (type == INIT_b){
        *APIC_ICR_LOW = 0x00000500;
    }else {
        *APIC_ICR_LOW = ((UINT32) 0x00000600 | (init_code_entry >> 12));
    }
    while ((*APIC_ICR_LOW) & (1<<12)){}
}

void (*foo)(void) = (void (*)())0x1000;//--------------------------------------------------

EFI_STATUS EFIAPI efi_main (IN EFI_HANDLE        ImageHandle, IN EFI_SYSTEM_TABLE  *SystemTable){
    STATIC EFI_MP_SERVICES_PROTOCOL  *mMpServices;
    EFI_STATUS Status;
    proc_info hdl;
    hdl.ProcNum = 0;
    hdl.ProcAllowed = 0;
    memory_map_t map_t = {NULL, 0, 0, 0, 0};

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&mMpServices);
    if (EFI_ERROR (Status)) {
        Print (L"Failed to find EFI_MP_SERVICES_PROTOCOL - %r\n", Status);
    return Status;
    }
    Status = mMpServices->GetNumberOfProcessors(mMpServices, &(hdl.ProcNum), &(hdl.ProcAllowed));
    Print (L"Processors - %d\nAllowed - %d\n", hdl.ProcNum, hdl.ProcAllowed);

    for (int i = 0; i < hdl.ProcNum; ++i){
        Status = mMpServices->GetProcessorInfo(mMpServices, i, &(hdl.Info));
        Print (L"Processor ID - %x\n", hdl.Info.ProcessorId);
        Print (L"Processor StatusFlag - %d\n", hdl.Info.StatusFlag);
    Print (L"Processor Location Package - %d\n", hdl.Info.Location.Package);
    Print (L"Processor Location Core- %d\n", hdl.Info.Location.Core);
    Print (L"Processor Location Thread - %d\n", hdl.Info.Location.Thread);
    Print (L"\n");
    }

    /*DEBUG*/
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_HANDLE *Handles;
    UINTN NoHandles;

    Status = SystemTable->BootServices->LocateHandleBuffer(ByProtocol,
                                                         &gEfiGraphicsOutputProtocolGuid,
                                                         NULL,
                                                         &NoHandles,
                                                         &Handles);
    if (EFI_ERROR (Status)) {
        Print(L"LocateHandleBuffer: %r\n", Status);
    return Status;
    }

    Status = SystemTable->BootServices->HandleProtocol (Handles[0],
                                                        &gEfiGraphicsOutputProtocolGuid,
                                                        (VOID **)&gop);
    if (EFI_ERROR (Status)) {
        Print(L"HandleProtocol: %r\n", Status);
    return Status;
    }
    /* GetMemoryMap */


    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, map_t.MemoryMapSize, (void **)&(map_t.MemoryMap));
    if (EFI_ERROR (Status)) {
        Print(L"AllocatePool: %r\n", Status);
    return Status;
    }

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    Status = gBS->LocateProtocol( &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(Status))
    Print(L"Unable to locate GOP");

    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = SystemTable->ConOut;


    conOut->OutputString(conOut, L"Some text that I want to display\r\n");
    Print(L"FrameBufferBase: %x\n", gop->Mode->FrameBufferBase);
    Print(L"FrameBufferBase: %d\n", gop->Mode->FrameBufferBase);
    Print(L"FrameBufferBase: %x\n", conOut);

    do{
        do{
            if(map_t.MemoryMap != NULL)
            Status = SystemTable->BootServices->FreePool(map_t.MemoryMap);
            if (EFI_ERROR(Status)) {
                Print(L"FreePool: %r\n", Status);
            return EFI_SUCCESS;
            }

            Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, map_t.MemoryMapSize, (void **)&(map_t.MemoryMap));
            if (EFI_ERROR (Status)) {
                Print(L"AllocatePool: %r\n", Status);
            return Status;
            }

            Status = SystemTable->BootServices->GetMemoryMap(&(map_t.MemoryMapSize),
                                                 map_t.MemoryMap,
                                                 &map_t.MapKey,
                                                 &map_t.DescriptorSize,
                                                 &map_t.DescriptorVersion);

        }while(Status != EFI_SUCCESS);
        Status = SystemTable->BootServices->ExitBootServices(ImageHandle, map_t.MapKey);

    }while(Status != EFI_SUCCESS);
// При выделении пула создается как минимум один новый дескриптор ... для фрагмента памяти, измененного на EfiLoaderData
// Не уверен, что прошивка UEFI должна выделяться на границе типа памяти ... если нет, то могут быть созданы два дескриптора

    asm_code_move();

    UINT32* tmp = (UINT32*)0x100;
    tmp[0]=123;

    for(int i = 1; i < hdl.ProcNum; ++i){
        INIT_SIPI_SIPI(INIT_b, 0, i);
    }

    for(int i = 1; i < 100000; ++i){}

    for(int i = 1; i < hdl.ProcNum; ++i){
        INIT_SIPI_SIPI(SIPI_b, Assembler_Code_offset, i);
    }

    for(int i = 1; i < 2000000; ++i){}

    for(int i = 1; i < hdl.ProcNum; ++i){
        INIT_SIPI_SIPI(SIPI_b, Assembler_Code_offset, i);
    }

//myPrint(gop->Mode->FrameBufferBase, 0xFF00);
//conOut->OutputString(conOut, L"Some text that I want to display\r\n");
    for(int i = 1; 1; ++i){
        myPrint(gop->Mode->FrameBufferBase, 0xFF00 + i);
        if(16711930 == i) i = 0;
    }
    while(1){}

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI EfiApp_Unload (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    return EFI_SUCCESS;
}

