$regfile = "m328pbdef.dat"
$crystal = 16000000
$hwstack = 250
$swstack = 250
$framesize = 250
$baud = 250000
$version 1 , 2 , 352



Config Scl = Porte.1
Config Sda = Porte.0
Config I2cdelay = 1
I2cinit
$lib "i2c.lbx"
G alias portb.5  : config g = output : reset g
bcl alias porte.2  : config bcl = output : reset bcl

i2c_en alias portc.2 : config i2c_en = output :  reset i2c_en
i2c_rst alias porte.3 : config i2c_rst = output :  reset i2c_rst



EN_PIN alias portd.6 : config EN_PIN = output            '10
DIN_PIN alias portd.7 : config DIN_PIN = output          '11
CLK_PIN alias portb.0 : config CLK_PIN = output          '12
STB_PIN alias portb.1 : config STB_PIN = output          '13
set EN_PIN
reset STB_PIN








dim var(63) as byte
dim count as byte

declare sub write_vfd(byval dat() as byte, byval data_size as byte)
declare sub write_command_vfd(byval com as byte)
declare sub set_brightness(byval value as byte)
declare sub bin_rect(arg1 As Byte , arg2 As Byte)
declare sub chr_str(arg1 as string*5)
declare sub dig_str(arg1 as string*6)
declare sub icon(byval arg1 as byte , byval stat as byte)
declare sub disk (byval stat as byte)
declare sub net_ring (byval stat1 as byte , byval stat2 as byte)
declare sub other_icons (byval stat1 as byte , byval stat2 as byte)


declare function scan_i2c() as byte
declare Sub Read_time(h_rtc As Byte , M_rtc As Byte , S_rtc As Byte )
Declare Sub Write_time(byval H_rtc As Byte , Byval M_rtc As Byte , Byval S_rtc As Byte )
Declare Sub Write_date(byval rtc_weekday As Byte , Byval rtc_date As Byte , Byval rtc_month As Byte, Byval rtc_year As Byte )
Declare Sub Read_date(rtc_weekday As Byte , rtc_date As Byte , rtc_month As Byte, rtc_year As Byte )
Declare sub rtc_ONint()


Config Adc = Single , Prescaler = Auto , Reference = internal       '???????????? ???
Dim Vbat As single , w as word
dim light as word , rtc_tmp as byte  , day as string*3



Config Clock = User                                         'Use USER to write/use your own code
Config Date = Dmy , Separator = .
'Date$ = "01.09.09"
'Time$ = "22:56:00"
call Write_time(22 , 29 , 10)
'call Write_date(4 , 2 , 10 , 21)
Config Com1 = 250000 , Synchrone = 0 , Parity = None , Stopbits = 1 , Databits = 8 , Clockpol = 0
Config Com2 = 38400 , Synchrone = 0 , Parity = None , Stopbits = 1 , Databits = 8 , Clockpol = 0

Dim S1 As String * 100 , Str_com1 As String * 100
Dim s_buf1(100) As Byte At S1 Overlay

Dim S2 As String * 100 , Str_com2 As String * 100
Dim s_buf2(100) As Byte At S2 Overlay

dim Str_com as string*100 , b2 as byte , serial_ready as bit

Declare function Com1_read() as string
Declare function Com2_read() as string

Config Serialin = Buffered , Size = 100
Config Serialin1 = Buffered , Size = 100  , Bytematch = All



Open "COM1:250000,8,n,1" For Binary As #1
Open "COM2:38400,8,n,1" For Binary As #2

print "RUN"

Dim Ar(7) As String * 10
Dim Bcount As Byte
dim update_icon_delay as byte


const trans = &hd1
const auto = &hd4
const epg = &hd7
const usb = &hdd
const mail = &he0
const title = &hce
const chp = &hc8
const trk = &hc5
const ch = &hc2
const rate = &hcb


var(1) = &h40
var(2) = &hc0
write_vfd var(1) , 49
write_command_vfd &h0a
write_command_vfd &h8f













dim tmp as byte , digs as byte
dim text_str as string*5
dim chr_buff(5) As Byte At text_str Overlay

dim digit_str as string*6 , dot as bit , dot_count as byte
dim dig_buff(6) As Byte At digit_str Overlay

dim tmp_str as string*12
Dim second_tick As Long
dim upd_req as bit



'icon trans , 1
'icon auto , 1
'icon epg , 1
'icon usb , 1
'icon mail , 1
'icon title , 1
'icon chp , 1
'icon trk , 1
'icon ch , 1
'icon rate , 1
'net_ring 255 , 255
'other_icons 255 , 255
'disk 255
set_brightness 7


call rtc_ONint

Start Adc
On Int0  Awake
Config Int0 = Falling
Enable Interrupts
Enable Int0





do
If Ischarwaiting() = 1 Then
    waitms 15
    Str_com = Com1_read()
    print #1 , s1
end if
if serial_ready = 1 then
reset serial_ready
Bcount = Split( Str_com , Ar(1) , "/")

print #1 , Ar(1)
print #1 , Ar(2)
print #1 , Ar(3)
print #1 , Ar(4)
print #1 , Ar(5)
print #1 , Ar(6)

if Ar(4) = "1" then
other_icons &b00001000 , 0
else
other_icons &b00000000 , 0
end if

Replacechars Ar(5) , "." , " "
if len(Ar(5)) < 5 then
do
Ar(5) = " " + Ar(5)
loop until  len(Ar(5)) => 5


end if
Time$ = Ar(3)
update_icon_delay = 3
set upd_req
end if

'If Ischarwaiting(#2) = 1 Then
'    waitms 15
'    Str_com = Com2_read()
'    print #1 , s2
'end if

   if upd_req = 0 then
      if update_icon_delay <> 0 then
      net_ring &b10000000 , 0
      else
      net_ring &b00000000 , 0
      end if
      'w = getadc(4)
      light = getadc(5)
      if light = 0 then
      set_brightness 1
      else
      set_brightness 7
      end if
            print #1 , tmp_str
      text_str = Ar(5)
      text_str = Ucase(text_str)
      tmp_str = Time$
      digit_str = mid(tmp_str , 1, 2)
      digit_str = digit_str + mid(tmp_str , 4, 2)
      digit_str = digit_str + mid(tmp_str , 7, 2)
      chr_str text_str
      dig_str tmp_str
      bin_rect _sec , _min
      set upd_req
   end if

   if dot_count > 0 then : decr dot_count : else
      if dot = 1 then
         reset dot
         reset upd_req
      end if
   end if

   waitms 10
loop


Serial1bytereceived:
B2 = Inkey(#2)
if b2 > 32 then
s2 = s2 + chr(b2)
end if

if b2 = &h0a then : Str_com = s2 : s2 = "" : set serial_ready : end if
Return







sub icon(byval arg1 as byte , byval stat as byte)
if stat => 1 then
stat = &hff
end if
var(1) = &h40
var(2) = arg1
var(3) = stat
write_vfd var(1) , 3
end sub

sub disk (byval stat as byte)
var(1) = &h40
var(2) = &hd8
var(3) = stat
write_vfd var(1) , 3
end sub

sub net_ring (byval stat1 as byte , byval stat2 as byte)
var(1) = &h40
var(2) = &hde
var(3) = stat1
var(4) = stat2
write_vfd var(1) , 4
end sub

sub other_icons (byval stat1 as byte , byval stat2 as byte)
var(1) = &h40
var(2) = &hd9
var(3) = stat1
var(4) = stat2
write_vfd var(1) , 4
end sub


sub chr_str(arg1 as string*5)
   for digs = 1 to 5
      if chr_buff(digs) = 32 then
      tmp = &h0a
      else
      tmp = chr_buff(digs) - 48
      end if
      var(1) = &h40
      var(2) = lookup(digs , char_order)
      var(3) = lookup(tmp , char_table_up)
      var(4) = lookup(tmp , char_table_low)
      write_vfd var(1) , 4
   next
end sub


sub dig_str(arg1 as string*6)
   for digs = 1 to 6
      tmp = dig_buff(digs) - 48
      var(1) = &h40
      var(2) = lookup(digs , digit_order)
      var(3) = lookup(tmp , digit_table)
      var(3).7 = dot
      write_vfd var(1) , 3
   next
end sub


sub bin_rect(arg1 As Byte , arg2 As Byte)
   var(1) = &h40
   var(2) = &hdb
   var(3) = arg1
   var(4) = arg2
   write_vfd var(1) , 4
end sub


sub write_vfd(byval dat() as byte, byval data_size as byte)
   local i as byte
   local n as byte

   STB_PIN=1
   for i = 0 to 7
      set CLK_PIN
      DIN_PIN = var(1).i
      reset CLK_PIN
   next
   STB_PIN=0
   nop

   if data_size > 1 then
      STB_PIN=1
      for n = 2 to data_size
         for i = 0 to 7
            set CLK_PIN
            DIN_PIN = var(n).i
            reset CLK_PIN
         next
      next
      STB_PIN=0
   end if
end sub

sub write_command_vfd(byval com as byte)
   local i as byte
   STB_PIN=1
   for i = 0 to 7
      set CLK_PIN
      DIN_PIN = com.i
      reset CLK_PIN
   next
   STB_PIN=0
   nop
end sub


sub set_brightness(byval value as byte)
   local tmp as byte
   tmp = &h88 + value
   write_command_vfd tmp
end sub

Sub Read_time(h_rtc As Byte , M_rtc As Byte , S_rtc As Byte )
   I2cstart
   I2cwbyte &h64
   I2cwbyte &H00
   I2cstart
   I2cwbyte &h65
   I2crbyte S_rtc , Ack                                        '0-59
   I2crbyte M_rtc , Ack                                        '0-59
   I2crbyte H_rtc , Nack                                        '0-23
   I2cstop
   S_rtc = Makedec(s_rtc) : M_rtc = Makedec(m_rtc) : H_rtc = Makedec(h_rtc)
End Sub

Sub Write_time(h_rtc As Byte , M_rtc As Byte , S_rtc As Byte)
   S_rtc = Makebcd(s_rtc) : M_rtc = Makebcd(m_rtc) : H_rtc = Makebcd(h_rtc)
   I2cstart
   I2cwbyte &h64
   I2cwbyte &H00
   I2cwbyte S_rtc
   I2cwbyte M_rtc
   I2cwbyte H_rtc
   I2cstop
End Sub

Sub Read_date(rtc_weekday As Byte , rtc_date As Byte , rtc_month As Byte , rtc_year As Byte)
   I2cstart
   I2cwbyte &h64
   I2cwbyte &H03
   I2cstart
   I2cwbyte &h65
   I2crbyte rtc_weekday , Ack                                     '0-31
   I2crbyte rtc_date , Ack                                        '0-6
   I2crbyte rtc_month , Ack                                       '1-12
   I2crbyte rtc_year , Nack                                       '0-99
   I2cstop
   rtc_weekday = Makedec(rtc_weekday) : rtc_date = Makedec(rtc_date) : rtc_month = Makedec(rtc_month) : rtc_year = Makedec(rtc_year)

End Sub

Sub Write_date(rtc_weekday As Byte , rtc_date As Byte , rtc_month As Byte, rtc_year As Byte )
   rtc_weekday = Makebcd(rtc_weekday) : rtc_date = Makebcd(rtc_date) : rtc_month = Makebcd(rtc_month) : rtc_year = Makebcd(rtc_year)
   I2cstart
   I2cwbyte &h64
   I2cwbyte &H03
   I2cwbyte rtc_weekday
   I2cwbyte rtc_date
   I2cwbyte rtc_month
   I2cwbyte rtc_year
   I2cstop
End Sub

sub rtc_ONint()

   I2cwbyte &h64
   I2cwbyte &H0b      'PERIODIC COUNTDOWN TIMER CONTROL REGISTERS
   I2cwbyte 64         'set 1 to timer counter 0
   I2cwbyte 0
   I2cstop

   I2cwbyte &h64
   I2cwbyte &H0d        'Extention register
   I2cwbyte &b00010001
   I2cstop

   I2cstart
   I2cwbyte &h64
   I2cwbyte &H0F
   I2cwbyte &b00110000    'TIE bit enable
   I2cstop
end sub





function Com1_read() as string
   $timeout = 100000
   Input  S1 Noecho
   'Com1_read = s1 '+ "<" + str(len(s))
end function

function Com2_read() as string
   $timeout = 100000
   Input #2 , S2 Noecho
   'Com2_read = s2 '+ "<" + str(len(s))
end function


function  scan_i2c()as byte
   dim bs as byte, stat as byte
   stat = 0
   Print "Scan start"
   For bs = 0 To 254 Step 2                                     'for all odd addresses
      I2cstart                                                 'send start
      I2cwbyte bs                                               'send address
      If Err = 0 Then                                           'we got an ack
         Print "Slave at : " ; bs ; " hex : " ; Hex(bs) ; " bin : " ; Bin(bs)
         incr stat
      End If
      I2cstop                                                  'free bus
   Next
   Print "End Scan"
   scan_i2c = stat
end function

Settime:
   call Write_time(_hour ,_min,_sec )
Return

Getdatetime:

   call Read_time(_hour ,_min,_sec )
   call Read_date(rtc_tmp ,_day ,_month,_year)
   'day = Lookupstr(rtc_tmp , daytab_ru_3)
Return

Setdate:
   call Write_date(rtc_tmp ,_day ,_month,_year)
Return

Awake:
   if update_icon_delay <> 0 then
   decr update_icon_delay
   end if
   dot_count = 50
   set dot
   reset upd_req
return
end



digit_order:
   data &h00,&hcf,&hd5,&hd6,&hd2,&hd3,&hd0
char_order:
   data &h00,&hcc,&hc9,&hc6,&hc3,&hc0,&hcf,&hd5,&hd6,&hd2,&hd3,&hd0
digit_table:
   data 119,18,107,91,30,93,125,19,127,31
char_table_up:
   data 49,24,209,209,240,225,225,17,241,241,0,0,0,0,0,0,0,241,213,33,149,161,161,225,240,132,16,168,32,186,178,49,241,177,241,225,133,48,168,176,138,138,137
char_table_low:
   data 70,2,69,66,3,67,71,2,71,67,0,0,0,0,0,0,0,7,82,68,82,69,5,70,7,16,70,37,68,6,38,70,5,102,37,67,16,70,12,46,40,16,72
daytab_ru:
   data "ÍÄ" , "ÏÍ" , "ÂT" , "ÑÐ" , "×Ò" , "ÏÒ" , "ÑÁ" , "ÍÄ"
daytab_ru_2:
   data "SU" , "MO" , "TU", "WE" , "TH" , "FR" , "SA" , "SU"
daytab_ru_3:
   data "SUN" , "MON" , "TUE", "WED" , "THU" , "FRI" , "SAT" , "SUN"