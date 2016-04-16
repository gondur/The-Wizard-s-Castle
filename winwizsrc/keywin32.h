//*********************************************************
//  KEYWIN32.H - Win32 keyboard scan codes                 
//                                                         
//  Compiled by:  Daniel D. Miller                         
//  Last Update:  12/28/01 14:18                           
//                                                         
//*********************************************************

//  these control keys may be OR'd into the other keycodes
#define  kShift      0x0100
#define  kCtrl       0x0200
#define  kAlt        0x0400

//  standard keys
#define  kBSPACE     0x0008
#define  kTAB        0x0009
#define  kENTER      0x000D
#define  kESC        0x001B
#define  kSPACE      0x0020
#define  kPGUP       0x0021
#define  kPGDN       0x0022
#define  kEND        0x0023
#define  kHOME       0x0024
#define  kLEFT       0x0025
#define  kUP         0x0026
#define  kRIGHT      0x0027
#define  kDOWN       0x0028
#define  kINS        0x002D
#define  kDEL        0x002E
#define  kMINUS      0x00BD
#define  kEQUALS     0x00BB

#define  kGHOME      (kHOME)
#define  kGEND       (kEND)
#define  kGPGUP      (kPGUP)
#define  kGPGDN      (kPGDN)
#define  kGRIGHT     (kRIGHT)
#define  kGLEFT      (kLEFT )
#define  kGUP        (kUP   )
#define  kGSTAR      (kSTAR )
#define  kGDOWN      (kDOWN )
#define  kGINS       (kINS  )
#define  kGDEL       (kDEL  )
#define  kGENTER     (kENTER)
#define  kGPlus      0x006B
#define  kGMinus     0x006D
#define  kGStar      0x006A
#define  kGSlash     0x006F

#define  k0          0x0030
#define  k1          0x0031
#define  k2          0x0032
#define  k3          0x0033
#define  k4          0x0034
#define  k5          0x0035
#define  k6          0x0036
#define  k7          0x0037
#define  k8          0x0038
#define  k9          0x0039

#define  ka          0x0041
#define  kb          0x0042
#define  kc          0x0043
#define  kd          0x0044
#define  ke          0x0045
#define  kf          0x0046
#define  kg          0x0047
#define  kh          0x0048
#define  ki          0x0049
#define  kj          0x004A
#define  kk          0x004B
#define  kl          0x004C
#define  km          0x004D
#define  kn          0x004E
#define  ko          0x004F
#define  kp          0x0050
#define  kq          0x0051
#define  kr          0x0052
#define  ks          0x0053
#define  kt          0x0054
#define  ku          0x0055
#define  kv          0x0056
#define  kw          0x0057
#define  kx          0x0058
#define  ky          0x0059
#define  kz          0x005A

#define  kF1         0x0070
#define  kF2         0x0071
#define  kF3         0x0072
#define  kF4         0x0073
#define  kF5         0x0074
#define  kF6         0x0075
#define  kF7         0x0076
#define  kF8         0x0077
#define  kF9         0x0078
#define  kF10        0x0079
#define  kF11        0x007A
#define  kF12        0x007B

#define  kSColon     0x00BA
#define  kComma      0x00BC
#define  kPeriod     0x00BE
#define  kSlash      0x00BF
#define  kBQuote     0x00C0
#define  kLBrkt      0x00DB
#define  kBSlash     0x00DC
#define  kRBrkt      0x00DD
#define  kSQuote     0x00DE

//  control and shift composite keys
#define  kA          (ka | kShift)
#define  kB          (kb | kShift)
#define  kC          (kc | kShift)
#define  kD          (kd | kShift)
#define  kE          (ke | kShift)
#define  kF          (kf | kShift)
#define  kG          (kg | kShift)
#define  kH          (kh | kShift)
#define  kI          (ki | kShift)
#define  kJ          (kj | kShift)
#define  kK          (kk | kShift)
#define  kL          (kl | kShift)
#define  kM          (km | kShift)
#define  kN          (kn | kShift)
#define  kO          (ko | kShift)
#define  kP          (kp | kShift)
#define  kQ          (kq | kShift)
#define  kR          (kr | kShift)
#define  kS          (ks | kShift)
#define  kT          (kt | kShift)
#define  kU          (ku | kShift)
#define  kV          (kv | kShift)
#define  kW          (kw | kShift)
#define  kX          (kx | kShift)
#define  kY          (ky | kShift)
#define  kZ          (kz | kShift)

#define  kCa         (ka | kCtrl)
#define  kCb         (kb | kCtrl)
#define  kCc         (kc | kCtrl)
#define  kCd         (kd | kCtrl)
#define  kCe         (ke | kCtrl)
#define  kCf         (kf | kCtrl)
#define  kCg         (kg | kCtrl)
#define  kCh         (kh | kCtrl)
#define  kCi         (ki | kCtrl)
#define  kCj         (kj | kCtrl)
#define  kCk         (kk | kCtrl)
#define  kCl         (kl | kCtrl)
#define  kCm         (km | kCtrl)
#define  kCn         (kn | kCtrl)
#define  kCo         (ko | kCtrl)
#define  kCp         (kp | kCtrl)
#define  kCq         (kq | kCtrl)
#define  kCr         (kr | kCtrl)
#define  kCs         (ks | kCtrl)
#define  kCt         (kt | kCtrl)
#define  kCu         (ku | kCtrl)
#define  kCv         (kv | kCtrl)
#define  kCw         (kw | kCtrl)
#define  kCx         (kx | kCtrl)
#define  kCy         (ky | kCtrl)
#define  kCz         (kz | kCtrl)

#define  kAq         (kq | kAlt)
#define  kAx         (kx | kAlt)

//  composite keys
#define  kQMark      (kSlash | kShift)
#define  kExclam     (k1 | kShift)
#define  kAtSign     (k2 | kShift)
#define  kPound      (k3 | kShift)
#define  kDollar     (k4 | kShift)
#define  kPercent    (k5 | kShift)
#define  kCaret      (k6 | kShift)
#define  kAmpersand  (k7 | kShift)
#define  kStar       (k8 | kShift)
#define  kLParen     (k9 | kShift)
#define  kRParen     (k0 | kShift)
#define  kDQuote     (kSQuote | kShift)

