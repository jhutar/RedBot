program ProjectX;   // Vit Valecka; Gymnazium JAK a JS s pravem SJZ Uhersky Brod

{$H+}
{$APPTYPE CONSOLE}

uses
  SysUtils;

const
  THEME = 0;   // 1 = výpisy zapnuté; 0 = bez výpisù
  CISLOHRACE = 1;   // pouze pro THEME = 1
  MAX = 100;
  VAKUUM = ' ';
  PRCEK = '1';
  OBR = '2';

var
  Vesmir: array [0..MAX,0..MAX] of string;     // (ne)koneèný vesmír
  ML1: array [1..2] of integer;                // pozice 1. lodì
  ML2: array [1..2] of integer;                // pozice 2. lodì
  Home: array [1..2] of integer;               // pozice domeèku
  Asteroidy: array [1..MAX,1..4] of integer;   // seznam pozic asteroidù
  SberAster: array [1..4] of integer;     // dva nejbližší asteroidy
  Vstup: TextFile;                   // vstupní TXT soubor
  RadekZdroje: string;               // ètený øádek zdrojového TXT
  MojeCislo: integer;                // moje "èíslo hráèe"
  RozmerX, RozmerY: integer;         // rozmìry "vesmíru"
  NL1: array [1..2] of integer;      // pozice 1. nepøátelské lodì
  NL2: array [1..2] of integer;      // pozice 2. nepøátelské lodì
  W: integer;              // globální promìnná zápisu
  PocetAsteroidu: integer;  // poèet asteroidù ve vesmíru

//------------------------------------------------------------------------------

procedure CtiVstup();
begin
 If ParamCount >= 1 then
  MojeCislo := StrToInt(ParamStr(1));
end;

//------------------------------------------------------------------------------

procedure RestartVesmiru();
var
 I, J: integer;
begin
 // velké èervené tlaèítko s nápisem "NEMAÈKAT!"
 For I:=0 to MAX do
  begin
   For J:=0 to MAX do
    begin
     Vesmir[I,J]:='';
    end;
  end;
 For I:=1 to MAX do
  begin
   For J:=1 to 4 do
    begin
     Asteroidy[I,J]:=0;
    end;
  end;
end;

//------------------------------------------------------------------------------

procedure CtiOHraci();
var
  I: integer;
  SMezi: string;
begin
 // ètení polohy základny
 SMezi:='';
 I:=1;
 repeat
  I:=I+1;
 until RadekZdroje[I-1] = '[';
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ',';
 If MojeCislo = W then
  Home[1]:=StrToInt(SMezi);
 SMezi:='';
 I:=I+1;
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ']';
 If MojeCislo = W then
  Home[2]:=StrToInt(SMezi);

 //ètení polohy první rakety
 SMezi:='';
 repeat
  I:=I+1;
 until RadekZdroje[I-1] = '[';
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ',';
 If MojeCislo = W then
  begin
   ML1[1]:=StrToInt(SMezi);
  end
 else
  begin
   NL1[1]:=StrToInt(SMezi);
  end;
 SMezi:='';
 I:=I+1;
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ']';
 If MojeCislo = W then
  begin
   ML1[2]:=StrToInt(SMezi);
  end
 else
  begin
   NL1[2]:=StrToInt(SMezi);
  end;

 //ètení polohy druhé rakety
 SMezi:='';
 I:=I+3;
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ',';
 If MojeCislo = W then
  begin
   ML2[1]:=StrToInt(SMezi);
  end
 else
  begin
   NL2[1]:=StrToInt(SMezi);
  end;
 SMezi:='';
 I:=I+1;
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ']';
 If MojeCislo = W then
  begin
   ML2[2]:=StrToInt(SMezi);
  end
 else
  begin
   NL2[2]:=StrToInt(SMezi);
  end;
end;

//------------------------------------------------------------------------------

procedure CtiSoubor();
var
 I, J: integer;
 SMezi: string;
begin
 // Pšššt! Nerušit!

 Assign(Vstup, 'playfield.txt');
 reset(Vstup);


 // PRVNÍ ØÁDEK - poèet kol - zatím neøešit
 readln(Vstup, RadekZdroje);


 // DRUHÝ ØÁDEK - rozmìry "vesmíru"
 readln(Vstup, RadekZdroje);
 SMezi:='';
 I:=1;
 // Xový rozmìr
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until RadekZdroje[I] = ' ';
 RozmerX:=StrToInt(SMezi);
 // Yový rozmìr
 SMezi:='';
 I:=I+1;
 repeat
  SMezi:=SMezi+RadekZdroje[I];
  I:=I+1;
 until I > length(RadekZdroje);
 RozmerY:=StrToInt(SMezi);


 // TØETÍ A ÈTVRTÝ ØÁDEK - O HRÁÈI
 W:=1;
 readln(Vstup, RadekZdroje);
 CtiOHraci;

 W:=2;
 readln(Vstup, RadekZdroje);
 CtiOHraci;


 // PÁTÝ ØÁDEK - odpovìï hráèe 1 - zatím neøešit
 readln(Vstup, RadekZdroje);


 // ŠESTÝ ØÁDEK - odpovìï hráèe 2 - zatím neøešit
 readln(Vstup, RadekZdroje);


 // HRACÍ POLE - ètení "vesmíru"
 For J:=0 to RozmerY-1 do
  begin
   readln(Vstup, RadekZdroje);
   For I:=0 to RozmerX-1 do
    begin
     Vesmir[I,J]:=RadekZdroje[I+1];
    end;
  end;

 Close(Vstup);
end;

//------------------------------------------------------------------------------

procedure KresliVesmir();
var
  I, J: integer;
begin
 For J:=0 to RozmerY-1 do
  begin
   For I:=0 to RozmerX-1 do
    begin
     write(Vesmir[I,J]);
    end;
   writeln;
  end;
end;

//------------------------------------------------------------------------------

procedure VypisInformace();
begin
 // rozumné, ale nepotøebné
 writeln('CisloHrace: ', CisloHrace);
 writeln('RozmerX: ', RozmerX);
 writeln('RozmerY: ', RozmerY);
 writeln('ML1: ', ML1[1], ',', ML1[2]);
 writeln('ML2: ', ML2[1], ',', ML2[2]);
 writeln('Home: ', Home[1], ',', Home[2]);
 writeln('NL1: ', NL1[1], ',', NL1[2]);
 writeln('NL2: ', NL2[1], ',', NL2[2]);
end;

//------------------------------------------------------------------------------

procedure Tahni();
begin
 If ML1[1] <> Home[1] then
  begin
   If ML1[1]>Home[1] then
    begin
     writeln('t l:t l');
     exit;
    end
   else
    begin
     writeln('t p:t p');
     exit
    end;
  end;

 If ML1[2] <> Home[2] then
  begin
   If ML1[2]>Home[2] then
    begin
     writeln('t n:t n');
     exit;
    end
   else
    begin
     writeln('t d:t d');
     exit;
    end;
  end;
end;

//------------------------------------------------------------------------------

procedure Let(X, Y: integer);
begin
 If ML1[1] <> X then                 // let první lodì
  begin
   If ML1[1]>X then                  // první se dorovnává X, pak Y
    begin
     If (ML1[1]-X) mod 2 = 0 then
      begin
       writeln('l l l:l l l');
       exit;
      end
     else
      begin
       writeln('l l:l l');
       exit;
      end;
    end
   else
    begin
     If (X-ML1[1]) mod 2 = 0 then
      begin
       writeln('l p p:l p p');
       exit;
      end
     else
      begin
       writeln('l p:l p');
       exit
      end;
    end;
  end;

 If ML1[2] <> Y then                 // let druhé lodì
  begin                              // první se dorovnává X, pak Y
   If ML1[2]>Y then
    begin
     If (ML1[2]-Y) mod 2 = 0 then
      begin
       writeln('l n n:l n n');
       exit;
      end
     else
      begin
       writeln('l n:l n');
       exit;
      end;
    end
   else
    begin
     If (Y-ML1[2]) mod 2 = 0 then
      begin
       writeln('l d d:l d d');
       exit;
      end
     else
      begin
       writeln('l d:l d');
       exit;
      end; 
    end;
  end;
end;

//------------------------------------------------------------------------------

procedure ETGoHome();
begin
 // støílení - zatím nic
end;

//------------------------------------------------------------------------------

procedure Mysli();
var
  X, Y: integer;
  I: integer;
  Vzdalenost: integer;
begin
 // Zzz...
 I:=1;
 For Y:=0 to RozmerY-1 do
  begin
   For X:=0 to RozmerX-1 do
    begin
     {If Vesmir[X,Y] = PRCEK then
      begin
       Asteroidy[I,1]:=X;
       Asteroidy[I,2]:=Y;
       Asteroidy[I,3]:=PRCEK;
      end;
     If Vesmir[X,Y] = OBR then
      begin
       Asteroidy[I,1]:=X;
       Asteroidy[I,2]:=Y;
       Asteroidy[I,3]:=OBR;
      end;  }
     If (Vesmir[X,Y] = OBR) or (Vesmir[X,Y] = PRCEK) then
      begin
       Asteroidy[I,1]:=X;
       Asteroidy[I,2]:=Y;
       If Vesmir[X,Y] = OBR then
        Asteroidy[I,3]:=2;
       If Vesmir[X,Y] = PRCEK then
        Asteroidy[I,3]:=1;
       Asteroidy[I,4]:=abs(ML1[1]-Asteroidy[I,1])+abs(ML1[2]-Asteroidy[I,2]);
       I:=I+1;
      end;
    end;
  end;
 PocetAsteroidu:=I-1;

 I:=1;
 Vzdalenost:=sqr(MAX);
 repeat
  // staèí øešit pouze jeden pøípad, lodì se pohybují spolu
  If Asteroidy[I,4] < Vzdalenost then
   begin
    Vzdalenost:=Asteroidy[I,4];
    SberAster[1]:=Asteroidy[I,1];
    SberAster[2]:=Asteroidy[I,2];
    SberAster[3]:=Asteroidy[I,3];
    SberAster[4]:=Asteroidy[I,4];
   end;
  I:=I+1;
 until (I=MAX) or (Asteroidy[I,3]=0);

 If Vzdalenost = 0 then
  begin
   If (SberAster[3]=2) and (ML1[1]=NL1[1]) and (ML1[2]=NL1[2]) then
    begin
     writeln('b:b');
     exit;
    end;
   Tahni;
  end;

 Let(SberAster[1],SberAster[2]);

 If THEME = 1 then
  writeln('Vzdalenost: ', Vzdalenost);
end;

//------------------------------------------------------------------------------

begin
 If THEME = 0 then
  CtiVstup;

 If THEME = 1 then
  MojeCislo:=CISLOHRACE;

 RestartVesmiru;
 CtiSoubor;

 If THEME = 1 then
  begin
   VypisInformace;
   KresliVesmir;
  end;

 Mysli;

 If THEME = 1 then
  begin
   writeln('PocetAsteroidu: ', PocetAsteroidu);
   readln;
  end; 
end.
