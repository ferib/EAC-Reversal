char __fastcall EAC::Callbacks::ProcessEnumerationScan(unsigned int *a1)
{
  __int64 v2; // rdi
  __int64 v4; // rcx
  unsigned int v5; // ecx
  char v6; // [rsp+20h] [rbp-48h]
  char v7[56]; // [rsp+30h] [rbp-38h] BYREF
  PVOID Object; // [rsp+78h] [rbp+10h] BYREF

  v6 = 0;
  v2 = 0i64;
  if ( !a1 )
    return 0;
  EAC::Callbacks::IsInUsermodeAddressSpace(a1, 16i64, 1);
  if ( !*a1 || !*(a1 + 1) || !a1[3] )
    return 0;
  if ( EAC::Imports::PsLookupProcessByProcessID(*a1, &Object) >= 0 )
  {
    if ( EAC::Callbacks::KeStackAttachProcess(Object, v7) )
    {
      LOBYTE(v4) = 1;
      v2 = EAC::Callbacks::ScanProcess(v4);
      EAC::Callbacks::KeUnstackDetachProcess(Object, v7);
    }
    ObfDereferenceObject(Object);
  }
  if ( v2 )
  {
    EAC::Callbacks::IsInUsermodeAddressSpace(*(a1 + 1), a1[3], 1);
    v5 = *(*v2 + 10i64);
    if ( v5 >= a1[3] )
      v5 = a1[3];
    EAC::Memory::memmove(*(a1 + 1), *v2, v5);
    v6 = 1;
    EAC::Memory::FreePoolAndUnicodeString(v2);
  }
  return v6;
}

struct_v6 *__fastcall EAC::Callbacks::ScanProcess(__int64 a1)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  bGetFileOnDisk = a1;
  bHasSetNeededData = 0;
  Process = EAC::Imports::PsGetCurrentProcess();
  pID = EAC::Imports::PsGetCurrentProcessID(Process);
  v5 = EAC::Memory::ExAllocatePoolWithRandomTag2(816i64);
  pStruct = v5;
  if ( v5 )
  {
    EAC::Memory::memset(v5, 0, 0x330ui64);
    v7 = EAC::Imports::GetProcessBaseAddress(Process);
    pStruct->qword8 = v7;
    if ( v7 || (v8 = EAC::Callbacks::GetUsermodeModuleWrapper(0i64), (pStruct->qword8 = v8) != 0i64) )
    {
      if ( EAC::Imports::IsWin32ConsoleSubsystem(Process) )
      {
        processFlags2 = 0x8001;
      }
      else if ( EAC::Imports::HasCOMDescriptor(pStruct->qword8) )
      {
        processFlags2 = 9;
      }
      else
      {
        v21[0] = 0x17B95E6A;
        v21[1] = 0xB5A7A4BD;
        v21[2] = 0x62CDCB21;
        v21[3] = 0xA77AB98D;
        v21[4] = 0xDE91C109;
        v21[5] = 0xE534B047;
        v22 = -19593;
        v11 = EAC::Memory::DecryptStringBuffer(v21, v20);
        v12 = EAC::Callbacks::GetUsermodeModuleWrapper(v11);
        memset(v20, 0, 0x1Aui64);
        v9 = 0i64;
        if ( v12 )
        {
          processFlags2 = 17;                   // msvbvm60.dll
        }
        else
        {
          v20[0] = 0x12D3327D;
          v20[1] = 0xCBBDD298;
          v20[2] = -1184356958;
          v20[3] = 0x10D2A830;
          v20[4] = 1441356086;
          v20[5] = -160030657;
          v13 = EAC::Memory::DecryptStringBuffer2(v20, v21);
          v14 = EAC::Callbacks::GetUsermodeModuleWrapper(v13);
          memset(v21, 0, sizeof(v21));
          v9 = 0i64;
          processFlags2 = 1;
          if ( v14 )
            processFlags2 = 0x1001;             // perl512.dll
        }
      }
      processFlags = processFlags2;
      v15 = pStruct->qword8;
      if ( EAC::Memory::IsDbgUiRemoteBreakinPatchedToCallLdrShutdownProcess(v9)
        || EAC::Memory::DoesProcessHaveAntiDebug(v15) )
      {
        processFlags2 |= 0x20u;
        processFlags = processFlags2;
      }
      if ( EAC::Callbacks::IsProcessVMProtected(pStruct->qword8) )
      {
        processFlags2 |= 0x40u;
        processFlags = processFlags2;
      }
      if ( EAC::Callbacks::HasDetectedPacker(pStruct->qword8, &nOutResult) )
        processFlags = (nOutResult != 0 ? 0x20000 : 0x10000) | processFlags2;
      EAC::Callbacks::SomeMaliciousIdentifierChecks(&processFlags);
      if ( bGetFileOnDisk
        && (EAC::Imports::GetDeviceName(Process, &pStruct[1].qword0 + 7)
         || EAC::Memory::GetMappedFilename(-1i64, pStruct->qword8, &pStruct[1].qword0 + 7)
         || EAC::Memory::GetProcessImageFilename(pID, &pStruct[1].qword0 + 7))
        || EAC::Memory::GetProcessPathOrCommandline(Process, 1, &pStruct[1].qword0 + 7) )
      {
        pStruct->bIsProgramOnDisk = 1;
        if ( EAC::Callbacks::IsMaliciousProgram(&pStruct[1].qword0 + 7) )
        {
          processFlags |= 0x40000u;
        }
        else if ( EAC::Callbacks::IsFileInSystemDirectory(&pStruct[1].qword0 + 7) )
        {
          processFlags |= 0x200u;
        }
      }
      EAC::Callbacks::IsInUsermodeAddressSpace(pStruct->qword8, 4096i64, 1);
      v16 = ((&pStruct[1].qword0 + 7) & -(pStruct->bIsProgramOnDisk != 0));
      v17 = EAC::Imports::PsGetProcessWow64(Process);
      pInfo = EAC::Memory::CopyProcessInformation(pStruct->qword8, 0x1000ui64, 0i64, processFlags, v17, v16, pID, 0i64);
      pStruct->qword0 = pInfo;
      if ( pInfo )
      {
        if ( !pStruct->bIsProgramOnDisk && EAC::Imports::GetProcessFileName(Process, v20) )
          EAC::Memory::CopyString(pStruct->qword0 + 0x20i64, 256i64, v20);
        bHasSetNeededData = 1;
      }
    }
  }
  if ( !bHasSetNeededData && pStruct )
  {
    EAC::Memory::FreePoolAndUnicodeString(pStruct);
    pStruct = 0i64;
  }
  return pStruct;
}

char __fastcall EAC::Memory::DoesProcessHaveAntiDebug(ULONG64 a1)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v2 = 0;
  v3 = 0;
  if ( !a1 )
    return 0;
  EAC::Callbacks::IsInUsermodeAddressSpace(a1, 4096i64, 1);
  if ( EAC::Memory::GetPEHeader(a1, 0x1000ui64, 0i64, &v12) )
  {
    v5 = v12;
    if ( *(v12 + 40) )
    {
      v6 = (*(v12 + 20) + v12 + 24);
      for ( i = 0; ; ++i )
      {
        if ( i >= *(v5 + 6) )
          return v2;
        EAC::Callbacks::IsInUsermodeAddressSpace(v6, 40i64, 1);
        v8 = v6->OptionalHeader.SizeOfUninitializedData;
        if ( v8 == 0x60000020 )                 // unpacked file
          break;
        if ( v8 == 0xE0000040 )                 // UPX packed exe? https://i.imgur.com/85R5QvD.png
        {
          if ( v6->Signature == '    ' && *&v6->FileHeader.Machine == '    ' )// cleared shit
            goto LABEL_11;
          for ( j = 0; ; ++j )
          {
            v11 = j == 8;
            if ( j >= 8 )
              break;
            if ( (*(&v6->Signature + j) - 97) > 25u )
            {
              v11 = j == 8;
              break;
            }
          }
          if ( v11 )
          {
            v9 = ++v3 == 2;
LABEL_10:
            if ( v9 )
              goto LABEL_11;
            goto LABEL_22;
          }
        }
LABEL_22:
        v6 = (v6 + 40);
      }
      if ( v6->Signature != '   ' )
        goto LABEL_22;
      v9 = *&v6->FileHeader.Machine == '    ';  // cleared shit
      goto LABEL_10;
    }
LABEL_11:
    v2 = 1;
  }
  return v2;
}

char __fastcall EAC::Callbacks::IsProcessVMProtected(ULONG64 BaseAddress)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v2 = 0;
  if ( !BaseAddress )
    return 0;
  EAC::Callbacks::IsInUsermodeAddressSpace(BaseAddress, 4096i64, 1);
  if ( EAC::Memory::GetPEHeader(BaseAddress, 0x1000ui64, 0i64, &v7) )
  {
    v4 = v7;
    v5 = (&v7->OptionalHeader + v7->FileHeader.SizeOfOptionalHeader);
    for ( i = 0; i < v4->FileHeader.NumberOfSections; ++i )
    {
      EAC::Callbacks::IsInUsermodeAddressSpace(v5, 40i64, 1);
      if ( *(&v5->e_magic + 1) == '0pmv' )
        return 1;
      v5 = (v5 + 40);
    }
  }
  return v2;
}

char __fastcall EAC::Callbacks::HasDetectedPacker(ULONG64 a1, _BYTE *a2)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v4 = 0;
  if ( a1 && a2 )
  {
    *a2 = 0;
    EAC::Callbacks::IsInUsermodeAddressSpace(a1, 4096i64, 1);
    if ( !EAC::Memory::GetPEHeader(a1, 0x1000ui64, 0i64, &v27) )
      return v4;
    v5 = v27;
    sectionName = (&v27->OptionalHeader.Magic + v27->FileHeader.SizeOfOptionalHeader);
    for ( i = 0; i < v5->FileHeader.NumberOfSections; ++i )
    {
      EAC::Callbacks::IsInUsermodeAddressSpace(sectionName, 40i64, 1);
      if ( *sectionName == '0XPU' )             // upx lol
        return 1;
      sectionName += 10;
    }
    v8 = EAC::Callbacks::IsEntrypointInUsermodeAddressSpace(a1, v5);
    if ( !v8 )
      return v4;
    nMachine = v5->FileHeader.Machine;
    if ( nMachine == 332 )
    {
      v10 = 32;
    }
    else
    {
      if ( nMachine == 0x8664 || nMachine == 0xAA64 )
      {
        if ( *v8 != 'UWVS' )
          return v4;
        v11 = &v8[sub_140020E6C(v8 + 4, v21) + 4];
        if ( (v26 & 0x1000) != 0 )
          return v4;
        if ( (v26 & 0x40000000) == 0 )
          return v4;
        if ( v22 != -115 )
          return v4;
        if ( v23 != 53 )
          return v4;
        sub_140020E6C(v11, v21);
        if ( (v26 & 0x1000) != 0 || (v26 & 0x40000000) == 0 || v22 != -115 || v24 != 7 )
          return v4;
        v12 = v25 == 6;
        goto LABEL_34;
      }
      v10 = 0;
    }
    if ( v10 != 32 )
      return v4;
    v13 = &v8[sub_14004CDFC(v8, v16)];
    if ( (v20 & 0x1000) != 0 )
      return v4;
    if ( v17 != 96 )
      return v4;
    v14 = &v13[sub_14004CDFC(v13, v16)];
    if ( (v20 & 0x1000) != 0 )
      return v4;
    if ( v17 != -66 )
      return v4;
    sub_14004CDFC(v14, v16);
    if ( (v20 & 0x1000) != 0 || v17 != -115 || v18 != 7 )
      return v4;
    v12 = v19 == 6;
LABEL_34:
    if ( v12 )
    {
      v4 = 1;
      *a2 = 1;
    }
    return v4;
  }
  return 0;
}

_OWORD *__fastcall EAC::Memory::CopyProcessInformation(unsigned __int64 a1, unsigned __int64 a2, unsigned __int64 a3, int a4, int a5, unsigned __int16 *a6, __int16 pID, _QWORD *a8)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v42 = a1;
  v9 = a2;
  v11 = 0i64;
  if ( a3 > 0x8000 )
    return 0i64;
  v12 = EAC::Memory::ExAllocatePoolWithRandomTag2(4704i64);
  v13 = v12;
  v40 = v12;
  if ( !v12 )
    return 0i64;
  EAC::Memory::memset(v12, 0, 0x260ui64);
  *&v13->char0 = 10;
  v13->dword4 = 1346718061;
  v13->word8 = 2;
  v13->qwordE = a1;
  v13->dword16 = v9;
  v13->dword1A = a4;
  v14 = 64;
  if ( a5 )
    v14 = a5;
  v13->byte1E = v14;
  v13->word220 = pID;
  if ( a6 )
  {
    v15 = *(a6 + 1);
    if ( v15 )
    {
      v16 = *a6;
      if ( v16 )
      {
        v17 = a6[1];
        if ( v17 )
        {
          v18 = &v13->word20;
          v19 = 0;
          if ( (v16 & 1) != 0 || (v17 & 1) != 0 || v16 > v17 || v17 == 0xFFFF )
            v19 = 0xC000000D;
          if ( v19 < 0 )
          {
            *v18 = 0;
          }
          else
          {
            v20 = v16 >> 1;
            v21 = 256i64;
            v22 = v15 - v18;
            do
            {
              if ( !(v20 + v21 - 256) )
                break;
              *v18 = *(v18 + v22);
              ++v18;
              --v21;
            }
            while ( v21 );
            v23 = v18 - 1;
            if ( v21 )
              v23 = v18;
            *v23 = 0;
          }
          if ( sub_140021608(a6, v37) )
            v13->byte1F = (*a6 - v37[0]) >> 1;
        }
      }
    }
  }
  v24 = &v13->oword260;
  if ( a1 && a2 >= 0x1000 && EAC::Memory::IsAddressLoadedInUsermodeMemory(a1, a2, &v13->gap222[18]) )
  {
    v25 = v13->word238;
    if ( v25 == 332 )
    {
      v26 = 32;
    }
    else if ( v25 == 0x8664 || v25 == 0xAA64 )
    {
      v26 = 64;
    }
    else
    {
      v26 = 0;
    }
    v13->byte1E = v26;
    if ( a1 < MmHighestUserAddress )
    {
      EAC::Callbacks::IsInUsermodeAddressSpace(a1, a2, 1);
      if ( EAC::Memory::GetPEHeader(a1, a2, 0i64, &v36) )
      {
        v27 = v36;
        EAC::Callbacks::CheckModuleExtension(v42, v13->byte1E, v36, v13->gap222);
        v39 = &v13->oword260;
        v28 = (v27 + *(v27 + 10) + 24);
        v38 = v28;
        v29 = 0;
        while ( v29 < *(v27 + 3) )
        {
          EAC::Callbacks::IsInUsermodeAddressSpace(v28, 40i64, 1);
          *v24 = *v28;
          ++v13->word254;
          ++v29;
          v28 += 5;
          v38 = v28;
          v24 = (v24 + 8);
          v39 = v24;
        }
        if ( EAC::Memory::CopyRawDataFromDebugDirectory(v42, v41, 128) )
        {
          v37[1] = v41;
          v30 = -1i64;
          do
            ++v30;
          while ( v41[v30] );
          v36 = v30;
          LOWORD(v37[0]) = v30;
          WORD1(v37[0]) = v30 + 1;
          EAC::Memory::memmove(v24, v41, v30);
          LODWORD(v24) = v30 + v24;
          v13->word25A = v30;
        }
      }
      else
      {
        v13->dword1A |= 0x80u;
      }
    }
    v31 = a3;
  }
  else
  {
    v13->dword1A |= 0x80u;
    v31 = a3;
  }
  v32 = v24 - v13;
  v13->unsigned_intA = v32;
  if ( v32 + v31 >= v32 )
  {
    v33 = EAC::Memory::ExAllocatePoolWithRandomTag2(v32 + v31);
    v11 = v33;
    if ( v33 )
    {
      EAC::Memory::memmove(v33, &v13->char0, v13->unsigned_intA);
      if ( v31 )
      {
        if ( a8 )
          *a8 = v11 + *(v11 + 10);
      }
    }
  }
  EAC::Memory::ExFreePool(v13);
  return v11;
}

char __fastcall EAC::Memory::IsImageOnDisk(__int64 a1, __int64 a2)
{
  char v2; // bl

  v2 = 0;
  if ( a1
    && a2
    && (EAC::Imports::GetDeviceName(a1, a2)
     || EAC::Callbacks::GetProcessName(a1, a2)
     || EAC::Memory::GetProcessPathOrCommandline(a1, 0, a2)) )
  {
    v2 = 1;
  }
  return v2;
}

void __fastcall EAC::Callbacks::ProtectGameProcess(unsigned int *a1)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v2 = 0;
  v68 = 0;
  v3 = 0i64;
  v4 = 0i64;
  v58[0] = 0;
  KeQueryTimeIncrement();
  if ( !a1 || *a1 == 197 )
    return;
  v5 = EAC::Memory::ExAllocatePoolWithRandomTag2(0x4000i64);
  v62 = v5;
  if ( !v5 )
    goto LABEL_85;
  v3 = EAC::Memory::ExAllocatePoolWithRandomTag2(1024i64);
  v63 = v3;
  if ( !v3 )
    goto LABEL_85;
  v6 = EAC::Memory::ExAllocatePoolWithRandomTag2(0x8000i64);
  v4 = v6;
  v64 = v6;
  if ( !v6 )
    goto LABEL_85;
  v59 = v6 + 2;
  v61 = v6 + 0x8000;
  if ( !EAC::Callbacks::KeStackAttachProcess(*(a1 + 1), v67) )
    goto LABEL_83;
  for ( i = 0i64; !*(a1 + 544) && EAC::Imports::NtQueryVirtualMemory(-1i64, i, 0i64, &i, 48i64); i += v54 )
  {
    v7 = (v54 + 4095) & 0xFFFFFFFFFFFFF000ui64;
    if ( !v7 )
      v7 = 4096i64;
    v54 = v7;
    v8 = v53;
    if ( v53 != i )
      goto LABEL_19;
    if ( v57 == 0x1000000 )
    {
      if ( !v53 || !EAC::Memory::PebImageByBaseWin64(v53) && !EAC::Memory::PebImageByBaseWow64(v8) )
      {
        v9 = EAC::Imports::GetProcessBaseAddress(*(a1 + 1));
        if ( v53 != v9 )
        {
          sub_140031F70(a1, &i);
          goto LABEL_35;
        }
      }
LABEL_19:
      if ( v57 == 0x1000000 && (v56 & 0x100) != 0 )
      {
        EAC::Callbacks::DetectLoadedModulesInGame(a1, 0x342B6272u, v53, 0i64);
        v34 = -1969993559;
        v35 = -1144243536;
        v36 = 242064136;
        v37 = 187676638;
        v38 = -1991878171;
        v39 = 96452294;
        v40 = -149557856;
        v41 = -694104463;
        v42 = -1557300779;
        v43 = -566847724;
        v44 = 510674766;
        v45 = 839459207;
        v46 = -379320832;
        v47 = 1465384362;
        v48 = 969356546;
        v49 = 1556836777;
        v50 = 307426473;
        v10 = v2 | 1;
        v12 = 0;
        if ( EAC::Memory::DecryptStr16(&v34, v66) )
        {
          v11 = *(a1 + 67);
          if ( v11 )
          {
            if ( !*(v11 + 32) )
              v12 = 1;
          }
        }
        v68 = v10 & 0xFFFFFFFE;
        memset(v66, 0, 0x44ui64);
        if ( v12 )
        {
          v34 = -1969993559;
          v35 = -1144243536;
          v36 = 242064136;
          v37 = 187676638;
          v38 = -1991878171;
          v39 = 96452294;
          v40 = -149557856;
          v41 = -694104463;
          v42 = -1557300779;
          v43 = -566847724;
          v44 = 510674766;
          v45 = 839459207;
          v46 = -379320832;
          v47 = 1465384362;
          v48 = 969356546;
          v49 = 1556836777;
          v50 = 307426473;
          v13 = EAC::Memory::DecryptStr16(&v34, v66);
          LODWORD(v33) = v55;
          LODWORD(v32) = v57;
          sub_14002B218(*(a1 + 67), v13, v54, v56, v32, v33);
          memset(v66, 0, 0x44ui64);
        }
        goto LABEL_35;
      }
    }
    if ( EAC::Imports::MmGetPhysicalForVirtual(i) )
    {
      v14 = *(a1 + 23);
      if ( v14 >= i && v14 < v54 + i )
        continue;
    }
    else if ( v54 <= 0x1000 || !EAC::Imports::MmGetPhysicalForVirtual(i - 4096 + v54) )
    {
      continue;
    }
LABEL_35:
    v15 = v54;
    v60 = v54;
    v65 = v54;
    v16 = v54;
    if ( v54 > 0x100000 )
      v16 = 0x100000i64;
    v54 = v16;
    EAC::Callbacks::IsInUsermodeAddressSpace(i, v16, 1);
    v17 = sub_14000F73C(&i, v5, 1024);
    v20 = v17;
    v69 = v17;
    if ( v17 )
    {
      nHASH = EAC::Memory::GetSomeHash(v18, 2, v5, v17);
      v22 = nHASH;
      if ( (nHASH != 0x1523ABE1 || v57 == 0x20000)// whitelisted module hashes? lol
        && (nHASH != 0x3EF9544D || v56 == 64)
        && (nHASH != 0x7D73439 || v57 == 0x20000) )
      {
        if ( nHASH == 0x1D73F827 )
        {
          if ( v57 == 0x1000000
            || (v23 = *a1, v23 <= 0x3E) && (v24 = 0x4002000800121000i64, _bittest64(&v24, v23))
            || v23 == 81
            || v23 == 89 )
          {
            v22 = 0;
          }
        }
        if ( v22 )
          goto SKIP_LOOP_LABEL;
      }
      v22 = 0;
      for ( j = 0; ; ++j )
      {
        v58[2] = j;
        if ( j >= v20 )
          break;
        v26 = 0;
        v27 = 0;
        v51 = 0i64;
        v28 = 0;
        for ( k = 0i64; ; k = (k + 1) )
        {
          v58[1] = k;
          LODWORD(v51) = v5[4 * j + 2];
          v29 = k == v51;
          if ( k >= v51 )
            break;
          v30 = *(k + *&v5[4 * j]);
          if ( v30 == 120 )
          {
            v26 |= 1u;
            v28 = 0;
            v51 = ++v27;
            if ( v27 == 3 )
              v26 |= 4u;
          }
          else
          {
            if ( v30 != 63 )
            {
              v29 = k == v51;
              break;
            }
            v26 |= 2u;
            v27 = 0;
            LODWORD(v51) = 0;
            HIDWORD(v51) = ++v28;
            if ( v28 == 2 )
              v26 |= 8u;
          }
        }
        if ( v29 && v26 == 15 )
        {
          v22 = 693986245;
          v20 = v69;
          break;
        }
        v20 = v69;
      }
      if ( v22 )
SKIP_LOOP_LABEL:
        sub_140031DC8(v22, &i, v5, v20, a1);
      v15 = v60;
    }
    if ( v54 > 0x1000 )
      v59 = EAC::Callbacks::SomeDisassemblerShitWrapper(&i, v3, k, v59, v61, v58);
    v2 = v68;
    v54 = v15;
  }
  EAC::Callbacks::KeUnstackDetachProcess(*(a1 + 1), v67);
  v31 = a1[10];
  if ( (v31 == 64 && i < 0x7FFFFFF0000i64 || v31 == 32 && i < 0x7FFE0000)
    && EAC::Imports::PsGetProcessExitProcessCalled(*(a1 + 1))
    && !*(a1 + 544) )
  {
    EAC::Callbacks::ReportViolation_1(a1, 0x49C7C002i64, &i, 8i64, 0i64);
  }
LABEL_83:
  if ( v58[0] )
  {
    *v4 = v58[0];
    EAC::Callbacks::ReportViolation_1(a1, 0x7A8B5AF5i64, v4, (v59 - v4), 0i64);
  }
LABEL_85:
  if ( v4 )
    EAC::Memory::ExFreePool(v4);
  if ( v3 )
    EAC::Memory::ExFreePool(v3);
  if ( v5 )
    EAC::Memory::ExFreePool(v5);
}

void __fastcall EAC::Callbacks::CheckForManualMappedModule(ULONG64 *pAnomalyHashStruct)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  if ( pAnomalyHashStruct )
  {
    v11 = 0i64;
    v2 = pAnomalyHashStruct[1];
    if ( v2 )
    {
      EAC::Callbacks::IsInUsermodeAddressSpace(pAnomalyHashStruct[1], 4096i64, 1);
      if ( EAC::Memory::GetPEHeader(v2, 0x1000ui64, 0i64, &v12) )
      {
        v3 = v12;
        v4 = &v12->OptionalHeader + v12->FileHeader.SizeOfOptionalHeader;
        for ( i = 0; i < v3->FileHeader.NumberOfSections && *(pAnomalyHashStruct + 10) != 32; ++i )
        {
          EAC::Callbacks::IsInUsermodeAddressSpace(v4, 40i64, 1);
          v6 = *(v4 + 3);
          if ( v6 )
          {
            v7 = *(v4 + 2);
            if ( v7 )
            {
              if ( (*(v4 + 9) & 0x2000000) == 0 )
              {
                v8 = *v4;
                if ( (*v4 != 'ler.' || *(v4 + 2) != 'co') && v8 != 'slt.' )
                {
                  if ( v8 != 'rsr.' || (v9 = 1, v4[4] != 'c') )
                    v9 = 0;
                  BYTE12(v11) = v9;
                  v10 = v9;
                  if ( (*(*pAnomalyHashStruct + 26) & 0x60) != 0 )
                    v10 = 1;
                  BYTE12(v11) = v10;
                  *&v11 = v2 + v6;
                  DWORD2(v11) = v7;
                  EAC::Callbacks::IsInUsermodeAddressSpace(v2 + v6, v7, 1);
                  EAC::Callbacks::SetHashOfAnomaly(&v11, pAnomalyHashStruct);
                }
              }
            }
          }
          v4 += 40;
        }
      }
    }
  }
}

void __fastcall EAC::Callbacks::DetectLoadedModulesInGame(__int64 a1, unsigned int a2, unsigned __int64 a3, __int64 a4)
{
  unsigned __int16 *v4; // rsi
  __int64 v7; // rax
  __int16 v8; // bx
  __int64 v9; // rax
  int v10; // eax
  unsigned int v11; // er12
  _OWORD *v12; // rax
  __int64 v13; // r14
  char *i; // rax
  __int64 v15; // rdx
  unsigned int v16; // ebx
  __int64 v17[2]; // [rsp+50h] [rbp-88h] BYREF
  char v18[16]; // [rsp+60h] [rbp-78h] BYREF
  char *v19; // [rsp+70h] [rbp-68h] BYREF
  char *v20; // [rsp+78h] [rbp-60h]
  unsigned __int64 v21; // [rsp+88h] [rbp-50h]
  int v22; // [rsp+98h] [rbp-40h]

  if ( a1 )
  {
    v4 = a4;
    if ( a3 )
    {
      if ( !a4 || !*(a4 + 8) || !*a4 || !*(a4 + 2) )
        v4 = (v18 & -(EAC::Memory::GetMappedFilename(-1i64, a3, v18) != 0));
      v7 = EAC::Imports::PsGetCurrentProcess();
      v8 = EAC::Imports::PsGetCurrentProcessID(v7);
      v9 = EAC::Imports::PsGetCurrentProcess();
      v10 = EAC::Imports::PsGetProcessWow64(v9);
      v11 = 0x8000;
      v12 = EAC::Memory::CopyProcessInformation(a3, 0x1000ui64, 0x8000ui64, 0, v10, v4, v8, v17);
      v13 = v12;
      v17[1] = v12;
      if ( v12 )
      {
        *(v12 + 22) = 0;
        for ( i = a3; ; i = &v19[v21] )
        {
          v19 = i;
          if ( !v11 )
            break;
          if ( !EAC::Imports::NtQueryVirtualMemory(-1i64, i, 0i64, &v19, 48i64) )
            break;
          if ( v20 != a3 )
            break;
          v15 = (v21 + 4095) & 0xFFFFFFFFFFFFF000ui64;
          v21 = v15;
          if ( !v15 )
            break;
          EAC::Callbacks::IsInUsermodeAddressSpace(v19, v15, 1);
          if ( v22 == 0x1000000 && v19 == v20 )
          {
            *(v13 + 26) |= 2u;
            if ( !EAC::Memory::GetImageBase(v19, v21) )
              *(v13 + 26) |= 0x80u;
          }
          v16 = v11;
          if ( v11 > v21 )
            v16 = v21;
          EAC::Memory::memmove((v17[0] + *(v13 + 22)), v19, v16);
          *(v13 + 22) += v16;
          v11 -= v16;
        }
        EAC::Callbacks::ReportViolation_1(a1, a2, v13, (*(v13 + 10) + *(v13 + 22)), 0i64);
        EAC::Memory::ExFreePool(v13);
      }
      if ( v4 == v18 )
        EAC::Memory::FreeUnicdeString(v18);
    }
  }
}

char __fastcall EAC::Callbacks::CheckForFrapsEXE(ULONG64 a1)
{
  char v2; // bl
  ULONG v3; // eax
  IMAGE_NT_HEADERS64 *v5; // [rsp+48h] [rbp+10h] BYREF

  v2 = 0;
  if ( a1 )
  {
    EAC::Callbacks::IsInUsermodeAddressSpace(a1, 4096i64, 1);
    if ( EAC::Memory::GetPEHeader(a1, 0x1000ui64, 0i64, &v5) )
    {
      if ( v5->FileHeader.Characteristics == 271 && v5->FileHeader.NumberOfSections == 5 )
      {
        v3 = v5->FileHeader.TimeDateStamp;
        if ( v3 == 0x4EA2A728 || v3 == 0x503F67EC || v3 == 0x512C56A2 || v3 == 0x55EAA341 )
          v2 = 1;
      }
    }
  }
  return v2;
}

char EAC::Callbacks::CheckRunningPrograms()
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v0 = 0;
  v1 = 0;
  v2 = 0;
  v3 = EAC::Memory::ExAllocatePoolWithRandomTag2(2048i64);
  v4 = v3;
  if ( !v3 )
    goto LABEL_23;
  v5 = EAC::Memory::GetRunningProcesses(v3, 0x100u);
  if ( v5 )
  {
    v6 = v4;
    while ( 1 )
    {
      if ( *v6 )
      {
        if ( EAC::Imports::PsLookupProcessByProcessID(*v6, &Object) >= 0 )
        {
          v7 = EAC::Imports::GetProcessFileName(Object, v55);
          ObfDereferenceObject(Object);
          if ( v7 )
          {
            v60 = 1551606071;
            v52 = 0i64;
            v8 = v1 | 1;
            v61 = 1300367728;
            v9 = -1293282290;
            for ( i = 0i64; i < 8; i += 4i64 )
            {
              v9 = __ROR4__(214013 * v9 + 2531011, 3);
              *(&v52 + i) = *(&v60 + i) ^ v9;
            }
            if ( EAC::Memory::CompareStrings(v55, &v52, 7ui64) )
              goto LABEL_15;
            v62 = 1587912774;
            v51 = 0i64;
            v8 |= 2u;
            v63 = 0x8CB32030;
            v11 = 0x97070F92;
            for ( j = 0i64; j < 8; j += 4i64 )
            {
              v11 = ~(((v11 ^ (v11 >> 7)) << 9) ^ v11 ^ (v11 >> 7) ^ ((((v11 ^ (v11 >> 7)) << 9) ^ v11 ^ (v11 >> 7)) >> 13));
              *(&v51 + j) = *(&v62 + j) ^ v11;
            }
            if ( EAC::Memory::CompareStrings(v55, &v51, 7ui64)// dbgview
                                                // devenv
                                                // tv_
              || (v8 |= 4u, strcpy(v59, "tv_"), EAC::Memory::CompareStrings(v55, v59, 3ui64)) )
            {
LABEL_15:
              v13 = 1;
            }
            else
            {
              v13 = 0;
            }
            if ( (v8 & 4) != 0 )
            {
              v8 &= 0xFFFFFFFB;
              memset(v59, 0, 4ui64);
            }
            if ( (v8 & 2) != 0 )
            {
              v8 &= 0xFFFFFFFD;
              memset(&v51, 0, sizeof(v51));
            }
            v1 = v8 & 0xFFFFFFFE;
            memset(&v52, 0, sizeof(v52));
            if ( v13 )
              break;
          }
        }
      }
      ++v0;
      ++v6;
      if ( v0 >= v5 )
        goto LABEL_22;
    }
    v2 = 1;
  }
LABEL_22:
  EAC::Memory::ExFreePool(v4);
  if ( !v2 )
  {
LABEL_23:
    v14 = EAC::Memory::GetRunningModules();
    v15 = v14;
    if ( v14 )
    {
      v16 = 0;
      if ( *v14 )
      {
        while ( 1 )
        {
          v17 = 74i64 * v16;
          if ( *&v15[v17 + 6] >= MmSystemRangeStart )
          {
            v18 = HIWORD(v15[v17 + 11]);
            v19 = &v15[v17 + 12];
            v20 = (v19 + v18) == 0i64;
            v21 = v19 + v18;
            String1.Buffer = v21;
            if ( v20 )
            {
              *&String1.Length = 0;
            }
            else
            {
              v22 = -1i64;
              do
                ++v22;
              while ( v21[v22] );
              String1.Length = v22;
              String1.MaximumLength = v22 + 1;
            }
            v42[0] = -1641674219;
            v23 = v1 | 8;
            v42[1] = -1284726641;
            *v48 = 0i64;
            v24 = -398366639;
            v48[8] = 0;
            v25 = 0i64;
            v43 = -64;
            do
            {
              v26 = v42[v25] ^ v24;
              v24 = __ROL4__(214013 * v24 + 2531011, 1);
              *&v48[v25 * 4] = v26;
              ++v25;
            }
            while ( v25 < 2 );
            v48[8] = v43 ^ v24;
            String2.Buffer = v48;
            v27 = -1i64;
            do
              ++v27;
            while ( v48[v27] );
            String2.Length = v27;
            String2.MaximumLength = v27 + 1;
            if ( !RtlCompareString(&String1, &String2, 1u) )// Dbgv.sys
              goto LABEL_51;
            v46[0] = -1717625671;
            v23 |= 0x10u;
            v46[1] = 1890075069;
            *v50 = 0i64;
            v28 = -623882519;
            *&v50[8] = 0;
            v29 = 0i64;
            *&v50[12] = 0;
            v46[2] = 1862005061;
            v47 = 31770;
            do
            {
              *&v50[v29 * 4] = v46[v29] ^ v28;
              ++v29;
              v28 = __ROL4__(
                      ((v28 ^ (v28 >> 7)) << 9) ^ v28 ^ (v28 >> 7) ^ ((((v28 ^ (v28 >> 7)) << 9) ^ v28 ^ (v28 >> 7)) >> 13),
                      2);
            }
            while ( v29 < 3 );
            for ( k = 12i64; k < 0xE; ++k )
            {
              v31 = v28;
              v28 >>= 8;
              v50[k] = *(v46 + k) ^ v31;
            }
            v57.Buffer = v50;
            v32 = -1i64;
            do
              ++v32;
            while ( v50[v32] );
            v57.Length = v32;
            v57.MaximumLength = v32 + 1;
            if ( !RtlCompareString(&String1, &v57, 1u) )// PROCMON23.sys
              goto LABEL_51;
            v44[0] = -448893557;
            v23 |= 0x20u;
            v44[1] = -928826563;
            *v49 = 0i64;
            v33 = -749400081;
            *&v49[8] = 0;
            v34 = 0i64;
            v45 = 15396;
            do
            {
              v35 = v44[v34] ^ v33;
              v33 = ~(214013 * v33 + 2531011);
              *&v49[v34 * 4] = v35;
              ++v34;
            }
            while ( v34 < 2 );
            for ( l = 8i64; l < 0xA; ++l )
            {
              v37 = v33;
              v33 >>= 8;
              v49[l] = *(v44 + l) ^ v37;
            }
            v58.Buffer = v49;
            v38 = -1i64;
            do
              ++v38;
            while ( v49[v38] );
            v58.Length = v38;
            v58.MaximumLength = v38 + 1;
            v39 = RtlCompareString(&String1, &v58, 1u);//  dbk64.sys
            v40 = 0;
            if ( !v39 )
LABEL_51:
              v40 = 1;
            if ( (v23 & 0x20) != 0 )
            {
              v23 &= 0xFFFFFFDF;
              memset(v49, 0, sizeof(v49));
            }
            if ( (v23 & 0x10) != 0 )
            {
              v23 &= 0xFFFFFFEF;
              memset(v50, 0, sizeof(v50));
            }
            v1 = v23 & 0xFFFFFFF7;
            memset(v48, 0, sizeof(v48));
            if ( v40 )
              break;
          }
          if ( ++v16 >= *v15 )
            goto LABEL_60;
        }
        v2 = 1;
      }
LABEL_60:
      EAC::Memory::ExFreePool(v15);
    }
  }
  return v2;
}

char *__fastcall EAC::Callbacks::DetectUsermodeAnomalies(unsigned int a1, __int64 a2, _OWORD *a3, __int64 a4, char *a5, unsigned int a6)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v20 = a3;
  v7 = EAC::Imports::PsGetCurrentProcess();
  result = EAC::Imports::GetProcessBaseAddress(v7);
  v9 = result;
  if ( result < MmHighestUserAddress )
  {
    v10 = (v19 & -(EAC::Callbacks::GetProcessName(v7, v19) != 0));
    v11 = EAC::Imports::PsGetCurrentProcessID(v7);
    v12 = EAC::Imports::PsGetCurrentProcess();
    v13 = EAC::Imports::PsGetProcessWow64(v12);
    v14 = a5;
    if ( a5 && a6 )
      v15 = a6 + 4096;
    else
      v15 = 4096;
    v16 = EAC::Memory::CopyProcessInformation(v9, 0x1000ui64, v15, 1, v13, v10, v11, &v20);
    v17 = v16;
    if ( v16 )
    {
      *(v16 + 22) = 0;
      if ( v9 )
      {
        EAC::Callbacks::IsInUsermodeAddressSpace((v9 + 4096), 4096i64, 1);
        EAC::Memory::memmove(v20, v9 + 4096, 0x1000ui64);
      }
      if ( !v14 )
        goto LABEL_14;
      if ( a6 && v20 )
        EAC::Memory::memmove(v20 + 256, v14, a6);
      v18 = a6;
      if ( !a6 )
LABEL_14:
        v18 = 0;
      EAC::Callbacks::ReportViolation(a1, v17, v18 + *(v17 + 10) + 4096);
      memset(v17, 0, (*(v17 + 10) + 4096));
      EAC::Memory::ExFreePool(v17);
    }
    result = v19;
    if ( v10 == v19 )
      result = EAC::Memory::FreeUnicdeString(v19);
  }
  return result;
}

__int64 __fastcall EAC::Callbacks::CheckForBlacklistedProcessNames(__int64 a1, _DWORD *a2, int a3, unsigned int a4)
{
  char v5; // bl
  char v8; // r14
  int v9; // edx
  unsigned __int64 v10; // rdi
  char v11; // dl
  unsigned int v12; // ecx
  int v13; // edx
  int v14; // ecx
  int v15; // edx
  unsigned int v16; // ecx
  bool v17; // zf
  unsigned int v18; // edx
  unsigned __int64 v19; // rdi
  unsigned __int64 i; // rdi
  char v21; // cl
  char v22; // dl
  unsigned int v23; // edx
  unsigned __int64 v24; // rdi
  int v25; // ecx
  unsigned __int64 j; // rdi
  char v27; // cl
  char v28; // dl
  unsigned int v29; // ecx
  int v30; // edx
  int v31; // edi
  int v32; // ecx
  int v33; // edx
  unsigned int v34; // edx
  unsigned __int64 v35; // rdi
  int v37[2]; // [rsp+38h] [rbp-49h]
  __int16 v38; // [rsp+40h] [rbp-41h]
  char v39; // [rsp+42h] [rbp-3Fh]
  int v40[3]; // [rsp+43h] [rbp-3Eh]
  char v41; // [rsp+4Fh] [rbp-32h]
  int v42[3]; // [rsp+50h] [rbp-31h]
  __int16 v43; // [rsp+5Ch] [rbp-25h]
  char v44; // [rsp+5Eh] [rbp-23h]
  int v45[3]; // [rsp+5Fh] [rbp-22h]
  _BYTE v46[11]; // [rsp+6Bh] [rbp-16h] BYREF
  _BYTE v47[13]; // [rsp+76h] [rbp-Bh] BYREF
  _BYTE v48[15]; // [rsp+83h] [rbp+2h] BYREF
  _BYTE v49[12]; // [rsp+92h] [rbp+11h] BYREF
  char v50[24]; // [rsp+A0h] [rbp+1Fh] BYREF

  v5 = 0;
  if ( !EAC::Imports::GetProcessFileName(a1, v50) )
    return a4;
  v8 = 1;
  if ( !a2 || *a2 != 0x52 && *a2 != 0x118 && *a2 != 0x119 && *a2 != 0x11B && *a2 != 0x17F && *a2 != 0x1EA )
    goto LABEL_13;
  v45[0] = -1709991619;
  *v49 = 0i64;
  v9 = 1210365784;
  *&v49[8] = 0;
  v10 = 0i64;
  v45[1] = 1457018111;
  v45[2] = 18902803;
  do
  {
    v9 = ~(1103515245 * v9 + 12345);
    *&v49[v10 * 4] = v45[v10] ^ v9;
    ++v10;
  }
  while ( v10 < 3 );
  v5 = 1;
  if ( EAC::Memory::CompareStrings(v50, v49, 0xBui64) )
  {
    v11 = 1;
  }
  else
  {
LABEL_13:
    v11 = 0;
    if ( (v5 & 1) == 0 )
      goto LABEL_26;
  }
  v5 &= 0xFEu;
  memset(v49, 0, sizeof(v49));
  if ( v11 )
  {
    v12 = a4 | 2;
    if ( (a3 & 2) == 0 )
      v12 = a4;
    v13 = v12 | 8;
    if ( (a3 & 8) == 0 )
      v13 = v12;
    v14 = v13 | 0x10;
    if ( (a3 & 0x10) == 0 )
      v14 = v13;
    v15 = v14 | 0x20;
    if ( (a3 & 0x20) == 0 )
      v15 = v14;
    v16 = v15 | 0x200;
    if ( (a3 & 0x200) == 0 )
      v16 = v15;
    a4 = v16 | 0x400;
    v17 = (a3 & 0x400) == 0;
LABEL_64:
    if ( v17 )
      a4 = v16;
    return a4;
  }
  if ( *a2 != 20 )
    goto LABEL_33;
LABEL_26:
  v37[0] = -1987792738;
  *v46 = 0i64;
  v5 |= 2u;
  *&v46[8] = 0;
  v18 = -640686133;
  v46[10] = 0;
  v19 = 0i64;
  v37[1] = 392935476;
  v38 = 14253;
  v39 = -74;
  do
  {
    *&v46[v19 * 4] = v37[v19] ^ v18;
    ++v19;
    v18 = __ROR4__(
            ((v18 ^ (v18 >> 7)) << 9) ^ v18 ^ (v18 >> 7) ^ ((((v18 ^ (v18 >> 7)) << 9) ^ v18 ^ (v18 >> 7)) >> 13),
            4);
  }
  while ( v19 < 2 );
  for ( i = 8i64; i < 0xB; ++i )
  {
    v21 = v18;
    v18 >>= 8;
    v46[i] = *(v37 + i) ^ v21;
  }
  if ( EAC::Memory::CompareStrings(v50, v46, 0xAui64) )
    v22 = 1;
  else
LABEL_33:
    v22 = 0;
  if ( (v5 & 2) != 0 )
  {
    v5 &= 0xFDu;
    memset(v46, 0, sizeof(v46));
  }
  if ( v22 )
  {
    v16 = a4 | 0x400;
    if ( (a3 & 0x400) == 0 )
      v16 = a4;
    a4 = v16 | 0x10;
    v17 = (a3 & 0x10) == 0;
    goto LABEL_64;
  }
  if ( *a2 != 330 )
    goto LABEL_47;
  v42[0] = 827781030;
  *v48 = 0i64;
  v5 |= 4u;
  *&v48[8] = 0;
  v23 = 1445236465;
  *&v48[12] = 0;
  v24 = 0i64;
  v48[14] = 0;
  v42[1] = -1132862917;
  v42[2] = 347640278;
  v43 = 26589;
  v44 = 98;
  do
  {
    v25 = v42[v24] ^ v23;
    v23 = __ROR4__(1103515245 * v23 + 12345, 3);
    *&v48[v24 * 4] = v25;
    ++v24;
  }
  while ( v24 < 3 );
  for ( j = 12i64; j < 0xF; ++j )
  {
    v27 = v23;
    v23 >>= 8;
    v48[j] = *(v42 + j) ^ v27;
  }
  if ( EAC::Memory::CompareStrings(v50, v48, 0xEui64) )
    v28 = 1;
  else
LABEL_47:
    v28 = 0;
  if ( (v5 & 4) != 0 )
  {
    v5 &= 0xFBu;
    memset(v48, 0, sizeof(v48));
  }
  if ( v28 )
  {
    v29 = a4 | 0x40;
    if ( (a3 & 0x40) == 0 )
      v29 = a4;
    v30 = v29 | 0x400;
    if ( (a3 & 0x400) == 0 )
      v30 = v29;
    v31 = v30 | 0x1000;
    if ( (a3 & 0x1000) == 0 )
      v31 = v30;
    v32 = v31 | 0x800;
    if ( (a3 & 0x800) == 0 )
      v32 = v31;
    v33 = v32 | 1;
    if ( (a3 & 1) == 0 )
      v33 = v32;
    v16 = v33 | 0x10;
    if ( (a3 & 0x10) == 0 )
      v16 = v33;
    a4 = v16 | 0x100000;
    v17 = (a3 & 0x100000) == 0;
    goto LABEL_64;
  }
  if ( MEMORY[0xFFFFF7800000026C] != 10 && EAC::Memory::GetBuildNumber() < 10240 )// if not windows 10
    goto LABEL_73;
  if ( (a3 & 0x400) == 0 )
    goto LABEL_73;
  v40[0] = -756454645;
  *v47 = 0i64;
  v5 |= 8u;
  *&v47[8] = 0;
  v34 = -1584920471;
  v47[12] = 0;
  v35 = 0i64;
  v40[1] = 1749006771;
  v40[2] = 72172906;
  v41 = -4;
  do
  {
    *&v47[v35 * 4] = v40[v35] ^ v34;
    ++v35;
    v34 = -(((v34 ^ (v34 << 13)) >> 7) ^ v34 ^ (v34 << 13) ^ ((((v34 ^ (v34 << 13)) >> 7) ^ v34 ^ (v34 << 13)) << 17));
  }
  while ( v35 < 3 );
  v47[12] = v41 ^ v34;
  if ( !EAC::Memory::CompareStrings(v50, v47, 0xCui64) )
LABEL_73:
    v8 = 0;
  if ( (v5 & 8) != 0 )
    memset(v47, 0, sizeof(v47));
  if ( v8 )
    a4 |= 0x400u;
  return a4;
}
