program FiddleJumper;

{$APPTYPE CONSOLE}

uses
  SysUtils;

Type Bod=record
  SouradniceX :integer;
  SouradniceY :integer;
end;

Type StatyHracu=record
  Zakladna :bod;
  Raketa1 :bod;
  Raketa2 :bod;
end;

Type Asteroid=record
  index :integer;
  SouradniceX :integer;
  SouradniceY :integer;
  VzdalenostOdMojiZakladny :integer;
  Vaha :integer;
end;
type konina=record
  indexy:array[1..999] of integer;
  Pocet :integer;
end;
var
  MojeID, NepritelovoID : integer;
  SirkaPole, VyskaPole, PocetAsteroidu :integer;
  Hrac :Array [1..2] of StatyHracu;
  PoleVesmiru :array[0..99, 0..99] of integer;
  AsteroidyVesmiru :array[1..999] of asteroid;
  PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny :Array[1..999] of integer;
  Pole :array[1..999] of konina;
  VzdalenostiKrizovatekOdZakladny :array[-1..102, -1..102] of integer;
  PoleVzdalenostiOdAsteroidu :array [-1..102, -1..102] of integer;
  CoMamDelatSPrvniLodi:string;
  CoMamDelatSDruhouLodi:string;

  procedure CtiVstup;
    begin
      If ParamCount >= 1 then
        MojeID := StrToInt(ParamStr(1))
      else
        MojeID := 1;

      NepritelovoID := 3 - MojeID;
    end;

  procedure SeradAsteroidy;
    var
      i, j :integer;
    begin
      for i:=1 to PocetAsteroidu do
        begin
          Pole[AsteroidyVesmiru[i].VzdalenostOdMojiZakladny].Pocet:=(Pole[AsteroidyVesmiru[i].VzdalenostOdMojiZakladny].Pocet)+1;
          Pole[AsteroidyVesmiru[i].VzdalenostOdMojiZakladny].indexy[Pole[AsteroidyVesmiru[i].VzdalenostOdMojiZakladny].Pocet]:=AsteroidyVesmiru[i].index;
        end;
      i:=1;
      j:=0;
      repeat
        if Pole[i].Pocet=0 then
          i:=i+1
        else
          begin
            j:=j+1;
            PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[j]:=Pole[i].indexy[Pole[i].Pocet];
            Pole[i].indexy[Pole[i].Pocet]:=0;
            Pole[i].Pocet:=Pole[i].Pocet-1;
          end;
      until j=PocetAsteroidu;
    end;

  procedure CtiZeSouboru;
    var
      Soubor :textfile;
      x, y, z:string;
      i, j, k, h:integer;
      SouradniceX, SouradniceY : integer;
    begin
      Assign(Soubor, 'playfield.txt');
      reset (Soubor);
      Readln (soubor, x);
      Readln (soubor, x);
      z:='';
      i:=1;
      repeat
        z:=z + x[i];
        i:=i+1;
      until x[i]=' ';
      SirkaPole:=StrToInt(z);
      y:='';
      repeat
        i:=i+1;
        y:=y + x[i];
      until i=Length(x);
      VyskaPole:=StrToInt(y);
      //i:=0;
      for i:=1 to 2 do
        begin
          Readln (soubor, x);
          k:=0;
          for j:=1 to 3 do
            begin
              y:='';
              z:='';
              repeat
                k:=k+1;
              until x[k] = '[';
              k:=k+1;
              repeat
                y:=y+x[k];
                k:=k+1;
              until x[k] = ',';
              k:=k+1;
              repeat
                z:=z+x[k];
                k:=k+1;
              until x[k] = ']';
              if j=1 then
                begin
                  Hrac[i].Zakladna.SouradniceX:=StrToInt(y);
                  Hrac[i].Zakladna.SouradniceY:=StrToInt(z);
                end;
              if j=2 then
                begin
                  Hrac[i].Raketa1.SouradniceX:=StrToInt(y);
                  Hrac[i].Raketa1.SouradniceY:=StrToInt(z);
                end;
              if j=3 then
                begin
                  Hrac[i].Raketa2.SouradniceX:=StrToInt(y);
                  Hrac[i].Raketa2.SouradniceY:=StrToInt(z);
                end;
            end;
        end;
      Readln (soubor, x);
      Readln (soubor, x);
      h:=0;
      For SouradniceY:=0 to (VyskaPole-1) do
        begin
          Readln (soubor, x);
          For SouradniceX:=0 to (SirkaPole-1) do
            begin
              VzdalenostiKrizovatekOdZakladny[SouradniceX, SouradniceY]:=(abs(Hrac[MojeID].Zakladna.SouradniceX-SouradniceX)+abs(Hrac[MojeID].Zakladna.SouradniceY-SouradniceY));
              if x[SouradniceX+1]<>' ' then
                begin
                  PoleVesmiru[SouradniceX, SouradniceY]:=StrToInt(x[SouradniceX+1]);
                  if (PoleVesmiru[SouradniceX, SouradniceY] = 1) or (PoleVesmiru[SouradniceX, SouradniceY] = 2) then
                    begin
                      h:=h+1;
                      PocetAsteroidu:=h;
                      AsteroidyVesmiru[h].index:=h;
                      AsteroidyVesmiru[h].SouradniceX:=SouradniceX;
                      AsteroidyVesmiru[h].SouradniceY:=SouradniceY;
                      AsteroidyVesmiru[h].Vaha:=PoleVesmiru[SouradniceX, SouradniceY];
                      AsteroidyVesmiru[h].VzdalenostOdMojiZakladny:=VzdalenostiKrizovatekOdZakladny[SouradniceX, SouradniceY];
                    end;
                end;
            end;
        end;
      close(Soubor);
      SeradAsteroidy;
    end;

    /////////////////////////////////////////////////////////////////////////

  procedure DopravaAsteroidu1;
    var
      i :integer;
    begin
      for i:=1 to PocetAsteroidu do
        begin
          if (Hrac[MojeId].Raketa1.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[MojeId].Raketa1.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa1.SouradniceX-1, Hrac[MojeId].Raketa1.SouradniceY]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and // overovani, zda je dane pole bliz k zakladne nez asteroid ,zda na poli uz neni asteroid
                  (PoleVesmiru[Hrac[MojeId].Raketa1.SouradniceX-1, Hrac[MojeId].Raketa1.SouradniceY]=0) and ((Hrac[MojeId].Raketa1.SouradniceX-1<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa1.SouradniceY<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa1.SouradniceX>0) then
                CoMamDelatSPrvniLodi:='t l';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa1.SouradniceX, Hrac[MojeId].Raketa1.SouradniceY+1]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa1.SouradniceX, Hrac[MojeId].Raketa1.SouradniceY+1]=0) and ((Hrac[MojeId].Raketa1.SouradniceX<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa1.SouradniceY+1<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa1.SouradniceY<VyskaPole-1) then
                CoMamDelatSPrvniLodi:='t d';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa1.SouradniceX+1, Hrac[MojeId].Raketa1.SouradniceY]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa1.SouradniceX+1, Hrac[MojeId].Raketa1.SouradniceY]=0) and ((Hrac[MojeId].Raketa1.SouradniceX+1<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa1.SouradniceY<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa1.SouradniceX<SirkaPole-1) then
                CoMamDelatSPrvniLodi:='t p';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa1.SouradniceX, Hrac[MojeId].Raketa1.SouradniceY-1]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa1.SouradniceX, Hrac[MojeId].Raketa1.SouradniceY-1]=0) and ((Hrac[MojeId].Raketa1.SouradniceX<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa1.SouradniceY-1<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa1.SouradniceY>0) then
                CoMamDelatSPrvniLodi:='t n';
            end;
        end;
    end;
  procedure DopravaAsteroidu2;
    var
      i :integer;
    begin
      for i:=1 to PocetAsteroidu do
        begin
          if (Hrac[MojeId].Raketa2.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[MojeId].Raketa2.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa2.SouradniceX-1, Hrac[MojeId].Raketa2.SouradniceY]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa2.SouradniceX-1, Hrac[MojeId].Raketa2.SouradniceY]=0) and ((Hrac[MojeId].Raketa2.SouradniceX-1<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa2.SouradniceY<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa2.SouradniceX>0) then
                CoMamDelatSDruhouLodi:='t l';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa2.SouradniceX, Hrac[MojeId].Raketa2.SouradniceY+1]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa2.SouradniceX, Hrac[MojeId].Raketa2.SouradniceY+1]=0) and ((Hrac[MojeId].Raketa2.SouradniceX<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa2.SouradniceY+1<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa2.SouradniceY<VyskaPole-1) then
                CoMamDelatSDruhouLodi:='t d';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa2.SouradniceX+1, Hrac[MojeId].Raketa2.SouradniceY]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa2.SouradniceX+1, Hrac[MojeId].Raketa2.SouradniceY]=0) and ((Hrac[MojeId].Raketa2.SouradniceX+1<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa2.SouradniceY<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa2.SouradniceX<SirkaPole-1) then
                CoMamDelatSDruhouLodi:='t p';
              if (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa2.SouradniceX, Hrac[MojeId].Raketa2.SouradniceY-1]<AsteroidyVesmiru[i].VzdalenostOdMojiZakladny) and
                  (PoleVesmiru[Hrac[MojeId].Raketa2.SouradniceX, Hrac[MojeId].Raketa2.SouradniceY-1]=0) and ((Hrac[MojeId].Raketa2.SouradniceX<>Hrac[NepritelovoID].Zakladna.SouradniceX) or
                  (Hrac[MojeId].Raketa2.SouradniceY-1<>Hrac[NepritelovoID].Zakladna.SouradniceY)) and (Hrac[MojeId].Raketa2.SouradniceY>0) then
                CoMamDelatSDruhouLodi:='t n';
            end;
        end;
    end;

  procedure UrciVzdalenostOdAsteroidu(pekneho:integer);
    var
      SouradniceX, SouradniceY :integer;
    begin
      For SouradniceY:=-2 to (VyskaPole+1) do
        begin
          For SouradniceX:=-2 to (SirkaPole+1) do
            begin
              PoleVzdalenostiOdAsteroidu[SouradniceX, SouradniceY]:=(abs(AsteroidyVesmiru[pekneho].SouradniceX-SouradniceX)+abs(AsteroidyVesmiru[pekneho].SouradniceY-SouradniceY));
            end;
        end;
    end;

  procedure JedProAsteroid1(index :integer);
    var
      i :integer;
    begin
      UrciVzdalenostOdAsteroidu(index);
      if Hrac[MojeID].Raketa1.SouradniceX=AsteroidyVesmiru[index].SouradniceX then   //svisle
        begin
          for i:=-2 to 2 do
            begin
              if (PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa1.SouradniceX, Hrac[MojeID].Raketa1.SouradniceY+i]<PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa1.SouradniceX, Hrac[MojeID].Raketa1.SouradniceY]) and (i<>0) then
                begin
                  if i=-2 then
                    CoMamDelatSPrvniLodi:='l n n';
                  if (i=-1) and (CoMamDelatSPrvniLodi<>'l n n') then
                    CoMamDelatSPrvniLodi:='l n';
                  if i=1 then
                    CoMamDelatSPrvniLodi:='l d';
                  if i=2 then
                    CoMamDelatSPrvniLodi:='l d d';
                end;
            end;
        end;

        if Hrac[MojeID].Raketa1.SouradniceY=AsteroidyVesmiru[index].SouradniceY then //vodorovne
        begin
          for i:=-2 to 2 do
            begin
              if (PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa1.SouradniceX+i, Hrac[MojeID].Raketa1.SouradniceY]<PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa1.SouradniceX, Hrac[MojeID].Raketa1.SouradniceY]) and (i<>0) then
                begin
                  if i=-2 then
                    CoMamDelatSPrvniLodi:='l l l';
                  if (i=-1) and (CoMamDelatSPrvniLodi<>'l l l') then
                    CoMamDelatSPrvniLodi:='l l';
                  if i=1 then
                    CoMamDelatSPrvniLodi:='l p';
                  if i=2 then
                    CoMamDelatSPrvniLodi:='l p p';
                end;
            end;
        end;
      if (Hrac[MojeID].Raketa1.SouradniceX<AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa1.SouradniceY<AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSPrvniLodi:='l d p';//vpravo dolu
      if (Hrac[MojeID].Raketa1.SouradniceX>AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa1.SouradniceY<AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSPrvniLodi:='l d l';//vlevo dole
      if (Hrac[MojeID].Raketa1.SouradniceX>AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa1.SouradniceY>AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSPrvniLodi:='l n l';//vlevo nahore
      if (Hrac[MojeID].Raketa1.SouradniceX<AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa1.SouradniceY>AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSPrvniLodi:='l n p';//vpravo nahore
    end;

  procedure JedProAsteroid2(index :integer);
    var
      i:integer;
    begin
      UrciVzdalenostOdAsteroidu(index);
      if Hrac[MojeID].Raketa2.SouradniceX=AsteroidyVesmiru[index].SouradniceX then    //svisle
        begin
          for i:=-2 to 2 do
            begin
              if (PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa2.SouradniceX, Hrac[MojeID].Raketa2.SouradniceY+i]<PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa2.SouradniceX, Hrac[MojeID].Raketa2.SouradniceY]) and (i<>0) then
                begin
                  if i=-2 then
                    CoMamDelatSDruhouLodi:='l n n';
                  if (i=-1) and (CoMamDelatSDruhouLodi<>'l n n') then
                    CoMamDelatSDruhouLodi:='l n';
                  if i=1 then
                    CoMamDelatSDruhouLodi:='l d';
                  if i=2 then
                    CoMamDelatSDruhouLodi:='l d d';
                end;
            end;
        end;

      if Hrac[MojeID].Raketa2.SouradniceY=AsteroidyVesmiru[index].SouradniceY then    //vodorovne
        begin
          for i:=-2 to 2 do
            begin
              if (PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa2.SouradniceX+i, Hrac[MojeID].Raketa2.SouradniceY]<PoleVzdalenostiOdAsteroidu[Hrac[MojeID].Raketa2.SouradniceX, Hrac[MojeID].Raketa2.SouradniceY]) and (i<>0) then
                begin
                  if i=-2 then
                    CoMamDelatSDruhouLodi:='l l l';
                  if (i=-1) and (CoMamDelatSDruhouLodi<>'l l l') then
                    CoMamDelatSDruhouLodi:='l l';
                  if i=1 then
                    CoMamDelatSDruhouLodi:='l p';
                  if i=2 then
                    CoMamDelatSDruhouLodi:='l p p';
                end;
            end;
        end;
      if (Hrac[MojeID].Raketa2.SouradniceX<AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa2.SouradniceY<AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSDruhouLodi:='l d p';//vpravo dolu
      if (Hrac[MojeID].Raketa2.SouradniceX>AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa2.SouradniceY<AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSDruhouLodi:='l d l';//vlevo dole
      if (Hrac[MojeID].Raketa2.SouradniceX>AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa2.SouradniceY>AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSDruhouLodi:='l n l';//vlevo nahore
      if (Hrac[MojeID].Raketa2.SouradniceX<AsteroidyVesmiru[index].SouradniceX) and (Hrac[MojeID].Raketa2.SouradniceY>AsteroidyVesmiru[index].SouradniceY) then
        CoMamDelatSDruhouLodi:='l n p';//vpravo nahore
    end;

  procedure Strilej1;
    var
      i :integer;
    begin
      for i:=1 to PocetAsteroidu do
        begin
          if (Hrac[NepritelovoID].Raketa1.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[NepritelovoID].Raketa1.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (Hrac[NepritelovoID].Raketa1.SouradniceX=Hrac[MojeID].Raketa1.SouradniceX) then
                begin
                  if (Hrac[NepritelovoID].Raketa1.SouradniceY<Hrac[MojeID].Raketa1.SouradniceY) then
                    CoMamDelatSPrvniLodi:='s n';
                  if (Hrac[NepritelovoID].Raketa1.SouradniceY>Hrac[MojeID].Raketa1.SouradniceY) then
                    CoMamDelatSPrvniLodi:='s d';
                end;
              if (Hrac[NepritelovoID].Raketa1.SouradniceY=Hrac[MojeID].Raketa1.SouradniceY) then
                begin
                  if (Hrac[NepritelovoID].Raketa1.SouradniceX<Hrac[MojeID].Raketa1.SouradniceX) then
                    CoMamDelatSPrvniLodi:='s l';
                  if (Hrac[NepritelovoID].Raketa1.SouradniceX>Hrac[MojeID].Raketa1.SouradniceX) then
                    CoMamDelatSPrvniLodi:='s p';
                end;
            end;
          if (Hrac[NepritelovoID].Raketa2.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[NepritelovoID].Raketa2.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (Hrac[NepritelovoID].Raketa2.SouradniceX=Hrac[MojeID].Raketa1.SouradniceX) then
                begin
                  if (Hrac[NepritelovoID].Raketa2.SouradniceY<Hrac[MojeID].Raketa1.SouradniceY) then
                    CoMamDelatSPrvniLodi:='s n';
                  if (Hrac[NepritelovoID].Raketa2.SouradniceY>Hrac[MojeID].Raketa1.SouradniceY) then
                    CoMamDelatSPrvniLodi:='s d';
                end;
              if (Hrac[NepritelovoID].Raketa2.SouradniceY=Hrac[MojeID].Raketa1.SouradniceY) then
                begin
                  if (Hrac[NepritelovoID].Raketa2.SouradniceX<Hrac[MojeID].Raketa1.SouradniceX) then
                    CoMamDelatSPrvniLodi:='s l';
                  if (Hrac[NepritelovoID].Raketa2.SouradniceX>Hrac[MojeID].Raketa1.SouradniceX) then
                    CoMamDelatSPrvniLodi:='s p';
                end;
            end;
        end;
    end;

  procedure Strilej2;
    var
      i :integer;
    begin
      for i:=1 to PocetAsteroidu do
        begin
          if (Hrac[NepritelovoID].Raketa1.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[NepritelovoID].Raketa1.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (Hrac[NepritelovoID].Raketa1.SouradniceX=Hrac[MojeID].Raketa2.SouradniceX) then
                begin
                  if (Hrac[NepritelovoID].Raketa1.SouradniceY<Hrac[MojeID].Raketa2.SouradniceY) then
                    CoMamDelatSDruhouLodi:='s n';
                  if (Hrac[NepritelovoID].Raketa1.SouradniceY>Hrac[MojeID].Raketa2.SouradniceY) then
                    CoMamDelatSDruhouLodi:='s d';
                end;
              if (Hrac[NepritelovoID].Raketa1.SouradniceY=Hrac[MojeID].Raketa2.SouradniceY) then
                begin
                  if (Hrac[NepritelovoID].Raketa1.SouradniceX<Hrac[MojeID].Raketa2.SouradniceX) then
                    CoMamDelatSDruhouLodi:='s l';
                  if (Hrac[NepritelovoID].Raketa1.SouradniceX>Hrac[MojeID].Raketa2.SouradniceX) then
                    CoMamDelatSDruhouLodi:='s p';
                end;
            end;
          if (Hrac[NepritelovoID].Raketa2.SouradniceX=AsteroidyVesmiru[i].SouradniceX) and (Hrac[NepritelovoID].Raketa2.SouradniceY=AsteroidyVesmiru[i].SouradniceY) then
            begin
              if (Hrac[NepritelovoID].Raketa2.SouradniceX=Hrac[MojeID].Raketa2.SouradniceX) then
                begin
                  if (Hrac[NepritelovoID].Raketa2.SouradniceY<Hrac[MojeID].Raketa2.SouradniceY) then
                    CoMamDelatSDruhouLodi:='s n';
                  if (Hrac[NepritelovoID].Raketa2.SouradniceY>Hrac[MojeID].Raketa2.SouradniceY) then
                    CoMamDelatSDruhouLodi:='s d';
                end;
              if (Hrac[NepritelovoID].Raketa2.SouradniceY=Hrac[MojeID].Raketa2.SouradniceY) then
                begin
                  if (Hrac[NepritelovoID].Raketa2.SouradniceX<Hrac[MojeID].Raketa2.SouradniceX) then
                    CoMamDelatSDruhouLodi:='s l';
                  if (Hrac[NepritelovoID].Raketa2.SouradniceX>Hrac[MojeID].Raketa2.SouradniceX) then
                    CoMamDelatSDruhouLodi:='s p';
                end;
            end;
        end;
    end;

  procedure Taktika;
    begin
      if (AsteroidyVesmiru[PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[1]].Vaha=AsteroidyVesmiru[PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[2]].Vaha) then
        begin
          JedProAsteroid1(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[1]);
          JedProAsteroid2(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[2]);
        end;
      if (AsteroidyVesmiru[PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[2]].Vaha=2) then
        begin
          JedProAsteroid1(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[2]);
          JedProAsteroid2(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[2]);
        end;
      if (AsteroidyVesmiru[PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[1]].Vaha=2) then
        begin
          JedProAsteroid1(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[1]);
          JedProAsteroid2(PoleSerazenychAsteroiduPodleVzdalenostiOdMojiZakladny[1]);
        end;

      DopravaAsteroidu1;  //Pokud ma lod 1 lezi na asteroidu, tak se ho pokusit dotahnout
      DopravaAsteroidu2;  //Pokud ma lod 2 lezi na asteroidu, tak se ho pokusit dotahnout
      if (PoleVesmiru[Hrac[MojeID].Raketa1.SouradniceX, Hrac[MojeID].Raketa1.SouradniceY]=1) and (PoleVesmiru[Hrac[MojeID].Raketa2.SouradniceX, Hrac[MojeID].Raketa2.SouradniceY]=1) and
          (VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa1.SouradniceX, Hrac[MojeId].Raketa1.SouradniceY]=VzdalenostiKrizovatekOdZakladny[Hrac[MojeId].Raketa2.SouradniceX, Hrac[MojeId].Raketa2.SouradniceY]) then
          CoMamDelatSDruhouLodi:='b';
      Strilej1;
      Strilej2;
      write(CoMamDelatSPrvniLodi, ':', CoMamDelatSDruhouLodi);
    end;

begin
  CtiVstup;
  CtiZeSouboru;
  Taktika;
  readln;
end.
