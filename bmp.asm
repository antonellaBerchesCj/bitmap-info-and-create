format ELF executable
segment readable executable

entry $               ; main
    call OpenFile
    ;call ReadFile
    call ReadHeader
    call PrintFileInfo


  mov eax, 1
  int 0x80            ; finish without errors


segment writeable executable
.data:
  inputFile               db    "tiny-256.bmp", 0
  fileDescriptor          dd    ?
  buffer                  db    1000000 dup(?),  0xA
  .sizeBuffer = $ - buffer
  ;BMP header
  magic                   dw    2
  .magicSize = $ - magic
  bfSize                  dd    4
  _rsvd                   dd    2
  pixel_offset            dd    4
  ; DIB header
  hdr_size                dd    4
  width                   dd    4
  height                  dd    4
  bpp                     dw    2
  compression             dd    4   ;0 for uncompressed images
  ncol                    dd    4

  ; acces structura elemente: bmp_header.hdr_size
.code:
  OpenFile:
      mov eax, 0x05                 ; sys_call open()
      mov ebx, inputFile
      mov ecx, 0                    ; file access mode - for read only access
      int  0x80                     ; call kernel
      mov [fileDescriptor], eax     ; put in fileDescriptor address the status
      cmp eax, 0                    ; if fileDescriptor exists
      jle ErrorMsgFunc              ; jump to ErrorMsgFunc
          ret

   ReadFile:
      mov eax, 0x03               ; sys_read
      mov ebx, [fileDescriptor]
      mov ecx, buffer
      mov edx, buffer.sizeBuffer
      int 0x80
          ret

   ReadHeader:
      ;read file header
      mov eax, 0x03               ; sys_read
      mov ebx, [fileDescriptor]
      mov ecx, magic
      mov edx, magic.magicSize
      int 0x80
          ret

   PrintFileInfo:
      mov eax, 4
      mov ebx, 1
      mov ecx, magic
      mov edx, magic.magicSize
      int 0x80

    CloseFile:
      mov eax, 0x06               ; sys_close
      mov ebx, [fileDescriptor]
          ret

    ErrorMsgFunc:
      mov ebx, 1
      mov ecx, errMsg
      mov edx, errMsg.size
      mov eax, 4
      int 80h

      mov eax, 1
      int 80h

      errMsg db "Cannot find the file!", 0xA   ;0xA --> new line
      .size = $ - errMsg
          ret
