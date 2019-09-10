#pragma once
#define WIN32_LEAN_MEAN
#ifndef MEMORYINFO_H
#define MEMORYINFO_H
#include <windows.h>
#include <Psapi.h>
#include <atlbase.h>
#include <dxgi1_4.h>
#include <iostream>
namespace util {
enum {
  KB = 10,
  MB = 20,
  GB = 30,
};
/*
	En threadsafe singleton som l�ter en utvecklare debugga och optimisera
	minneshanteringen av applikationen
*/
class MemoryInfo {
 public:
  static MemoryInfo& GetInstance() {
    static MemoryInfo instance;
    return instance;
  }
  unsigned GetUsedRAM(unsigned short suffix = MB) {
    GetProcessMemoryInfo(GetCurrentProcess(), &PMC_, sizeof(PMC_));
    return (unsigned)(PMC_.WorkingSetSize >> suffix);
  }
  /*
	Returnerar hur mycket VRAM processen anv�nder sig utav
	via ett windows/directX api. 
  */
  int GetUsedVRAM(unsigned short suffix = MB) {
	  //Om funktionen kallas mer �n en g�ng s� m�ste vi sl�ppa p� f�reg�ende 
	  //Devices f�r att undvika memoryleaks.
    if (Curr_Adapter_ || Adapters_) {
      Dx_factory_->Release();
      Adapters_->Release();
    }
	//Adapters inneh�ller alla renderings-enheter, b�de fysiska och virtuella 
    Curr_Adapter_ = nullptr;
    Adapters_ = nullptr;
	//Vi vet att vi enbart vill ha den f�rsta fysiska enheten, s� vi skippar att loopa
    Dx_factory_->EnumAdapters1(0, &Curr_Adapter_);
    DXGI_ADAPTER_DESC1 desc;
    Curr_Adapter_->GetDesc1(&desc);
    if (!Adapters_ && desc.Flags == 0) {
      Curr_Adapter_->QueryInterface(IID_PPV_ARGS(&Adapters_));
	  //F� fram minnesinfo fr�n adaptern.
      DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
      Adapters_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
                                      &info);
	  //Returnera m�ngden anv�nt minne eller -1 om det g�r fel.
      return (int)(info.CurrentUsage >> suffix);
    } else {
      return -1;
    }
  }
  /*/
	Samma princip som ovan, fast returnera hur mycket minne som finns p� enheten
  */
  int GetVRAMAmount(unsigned short suffix = MB) {
    if (Curr_Adapter_ || Adapters_) {
      Dx_factory_->Release();
      Adapters_->Release();
    }
    Curr_Adapter_ = nullptr;
    Adapters_ = nullptr;
    Dx_factory_->EnumAdapters1(0, &Curr_Adapter_);
    DXGI_ADAPTER_DESC1 desc;
    Curr_Adapter_->GetDesc1(&desc);
    if (!Adapters_ && desc.Flags == 0) {
      Curr_Adapter_->QueryInterface(IID_PPV_ARGS(&Adapters_));
      DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
      Adapters_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
                                      &info);
      return (int)(info.Budget >> suffix);
    } else {
      return -1;
    }
  }

 private:
  MemoryInfo() { CreateDXGIFactory1(IID_PPV_ARGS(&Dx_factory_)); }
  ~MemoryInfo() {
    Dx_factory_->Release();
    Adapters_->Release();
  }
  MemoryInfo(const MemoryInfo&) = delete;
  MemoryInfo& operator=(const MemoryInfo&) = delete;
  MemoryInfo(MemoryInfo&&) = delete;
  MemoryInfo& operator=(MemoryInfo&&) = delete;

  //DX-variabler f�r GPU information.
  IDXGIFactory4* Dx_factory_ = nullptr;
  IDXGIAdapter3* Adapters_ = nullptr;
  IDXGIAdapter1* Curr_Adapter_ = nullptr;
  PROCESS_MEMORY_COUNTERS PMC_ = {};
};
}  // namespace util
#endif  // !MEMORYINFO_H