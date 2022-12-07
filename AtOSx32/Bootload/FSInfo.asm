;|===============================================FS-Info=============================================================|
FSsignature1      dd 0x41615252  ; Signature                                                                         |
times 0x1E0       db 0x0         ; Pad 480 reserved bytes                                                            |
FSsignature2      dd 0x61417272  ; Signature                                                                         |
LastClusterCount  dd 0xFFFFFFFF  ; Since this might always be incorrect, indicate that this field does not matter    |
FirstDataCluster  dd 0x2         ; Also not important                                                                |
times 0xC         db 0x0         ; Padding reserved 12 bytes                                                         |
TrailSignature    dd 0xAA550000  ; Trail signature                                                                   | 
;|===============================================FS-Info=============================================================|

