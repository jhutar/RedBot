program jfoltaBot;

{$mode objfpc}{$H+}

uses {$IFDEF UNIX} {$IFDEF UseCThreads}
  cthreads, {$ENDIF} {$ENDIF}
  Classes,
  strutils,
  SysUtils { you can add units after this };

type
  TCoordinate = record
    XCoordinate, YCoordinate: integer;
  end;

  TRocket = record
    RocketCoordinate, ClosestAsteriodCoordinate: TCoordinate;
    HasAsteriod, CanBeShot, IsCloserToAsteriod: boolean;
    ShootingDirection, ClosestAsteriodDirection, BaseDirection,
    FriendlyRocketDirection: string;
  end;

  TBase = record
    BaseCoordinate: TCoordinate;
  end;

  TPlayer = record
    Rocket1, Rocket2: TRocket;
    Base: TBase;
    IsEnemy: boolean;
    AsteriodList: array of TCoordinate;
  end;

const
  GAME_FILE_NAME = 'playfield.txt';

var
  GamePlan: array of array of string;
  //data representation: lower case - me, upper case - enemy, B, b - base, R, r - Rockets, 1, 2 - asteriods
  XSize, YSize: integer;
  Player1, Player2, MyPlayer: TPlayer;
  Command: string;

  function GetFirstNumber(var s: string): integer;
  var
    i: integer;
    TempString: string;
  begin
    Result := 0;
    i := 1;
    TempString := '';
    while not (s[i] in ['0'..'9']) and (i <= Length(s)) do
      Inc(i);
    while (s[i] in ['0'..'9']) and (i <= Length(s)) do
    begin
      TempString := TempString + s[i];
      Inc(i);
    end;
    if TempString <> '' then
      Result := StrToInt(TempString);
    s := Copy(s, i, Length(s));
  end;

  procedure IndicateBaseAndRockets(p: TPlayer);
  var
    TempBase, TempRocket: string;
  begin
    TempBase := 'b';
    TempRocket := 'r';
    if p.IsEnemy then
    begin
      TempBase := UpperCase(TempBase);
      TempRocket := UpperCase(TempRocket);
    end;
    with p do
    begin
      AppendStr(GamePlan[Base.BaseCoordinate.XCoordinate,
        Base.BaseCoordinate.YCoordinate], TempBase);
      AppendStr(GamePlan[Rocket1.RocketCoordinate.XCoordinate,
        Rocket1.RocketCoordinate.YCoordinate], TempRocket);
      AppendStr(GamePlan[Rocket2.RocketCoordinate.XCoordinate,
        Rocket2.RocketCoordinate.YCoordinate], TempRocket);
    end;
  end;

  procedure LookForTargets(var r: TRocket);
  var
    i: integer;
    HasFoundFriendly: boolean;
  begin
    r.CanBeShot := False;
    r.ShootingDirection := '';
    HasFoundFriendly := False;
    for i := r.RocketCoordinate.XCoordinate + 1 to XSize - 1 do
      // right
    begin
      if AnsiContainsStr(GamePlan[i, r.RocketCoordinate.YCoordinate], 'r') then
        HasFoundFriendly := True;
      if (AnsiContainsStr(GamePlan[i, r.RocketCoordinate.YCoordinate], 'R')) and
        not HasFoundFriendly then
      begin
        r.CanBeShot := True;
        AppendStr(r.ShootingDirection, 'p');
      end;
    end;
    HasFoundFriendly := False;
    for i := r.RocketCoordinate.XCoordinate - 1 downto 0 do
      // left
    begin
      if AnsiContainsStr(GamePlan[i, r.RocketCoordinate.YCoordinate], 'r') then
        HasFoundFriendly := True;
      if (AnsiContainsStr(GamePlan[i, r.RocketCoordinate.YCoordinate], 'R')) and
        not HasFoundFriendly then
      begin
        r.CanBeShot := True;
        AppendStr(r.ShootingDirection, 'l');
      end;
    end;
    HasFoundFriendly := False;
    for i := r.RocketCoordinate.YCoordinate - 1 downto 0 do
      //up
    begin
      if AnsiContainsStr(GamePlan[r.RocketCoordinate.XCoordinate, i], 'r') then
        HasFoundFriendly := True;
      if (AnsiContainsStr(GamePlan[r.RocketCoordinate.XCoordinate, i], 'R')) and
        not HasFoundFriendly then
      begin
        r.CanBeShot := True;
        AppendStr(r.ShootingDirection, 'n');
      end;
    end;
    HasFoundFriendly := False;
    for i := r.RocketCoordinate.YCoordinate + 1 to YSize - 1 do
      // down
    begin
      if AnsiContainsStr(GamePlan[r.RocketCoordinate.XCoordinate, i], 'r') then
        HasFoundFriendly := True;
      if (AnsiContainsStr(GamePlan[r.RocketCoordinate.XCoordinate, i], 'R')) and
        not HasFoundFriendly then
      begin
        r.CanBeShot := True;
        AppendStr(r.ShootingDirection, 'd');
      end;
    end;
  end;

  //    x0 x1 x2 x3 ...
  // y0 r1    r2
  // y1
  // y2       R2
  // y3       R1
  // ..
  function GetPath(c1, c2: TCoordinate): string;
  begin
    Result := '';
    while (c1.XCoordinate <> c2.XCoordinate) or (c1.YCoordinate <> c2.YCoordinate) do
    begin
      if c1.XCoordinate - c2.XCoordinate > 0 then
      begin
        Result := Result + 'l';
        Dec(c1.XCoordinate);
      end;
      if c1.XCoordinate - c2.XCoordinate < 0 then
      begin
        Result := Result + 'p';
        Inc(c1.XCoordinate);
      end;
      if c1.YCoordinate - c2.YCoordinate > 0 then
      begin
        Result := Result + 'n';
        Dec(c1.YCoordinate);
      end;
      if c1.YCoordinate - c2.YCoordinate < 0 then
      begin
        Result := Result + 'd';
        Inc(c1.YCoordinate);
      end;
    end;
  end;

  procedure DebugListPlayer(p: TPlayer);
  var
    x, y: integer;
  begin
    for y := 0 to YSize - 1 do
    begin
      WriteLn(y);
      for x := 0 to XSize - 1 do
      begin
        Write('[', x, ':', y, ']:');
        Writeln(GamePlan[x, y]);
      end;
      WriteLn();
    end;
    with p do
    begin
      Writeln('R1 [', Rocket1.RocketCoordinate.XCoordinate, ':',
        Rocket1.RocketCoordinate.YCoordinate, ']');
      Writeln('R2 [', Rocket2.RocketCoordinate.XCoordinate, ':',
        Rocket2.RocketCoordinate.YCoordinate, ']');
      Writeln('Base [', Base.BaseCoordinate.XCoordinate, ':',
        Base.BaseCoordinate.YCoordinate, ']');
      WriteLn('R1 Base direction: ', Rocket1.BaseDirection);
      WriteLn('R2 Base direction: ', Rocket2.BaseDirection);
      WriteLn('R1 Closest A direction: ', Rocket1.ClosestAsteriodDirection);
      Writeln('R1 Closest A [', Rocket1.ClosestAsteriodCoordinate.XCoordinate, ':',
        Rocket1.ClosestAsteriodCoordinate.YCoordinate, ']');
      WriteLn('R2 Closest A direction: ', Rocket2.ClosestAsteriodDirection);
      Writeln('R2 Closest A [', Rocket2.ClosestAsteriodCoordinate.XCoordinate, ':',
        Rocket2.ClosestAsteriodCoordinate.YCoordinate, ']');
      WriteLn('R1 Shooting direction: ', Rocket1.ShootingDirection);
      WriteLn('R2 Shooting direction: ', Rocket2.ShootingDirection);
    end;
  end;

  procedure GetAsteriodCoordinates(var p: TPlayer);
  var
    x, y, i: integer;
    TempStr: string;
  begin
    //create a full list of all available asteriods from game plan
    SetLength(p.AsteriodList, 0);
    for y := 0 to YSize - 1 do
      for x := 0 to XSize - 1 do
      begin
        if (AnsiContainsStr(GamePlan[x, y], '1')) or
          (AnsiContainsStr(GamePlan[x, y], '2')) then
        begin
          SetLength(p.AsteriodList, Length(p.AsteriodList) + 1);
          p.AsteriodList[Length(p.AsteriodList) - 1].XCoordinate := x;
          p.AsteriodList[Length(p.AsteriodList) - 1].YCoordinate := y;
        end;
      end;
    //Finds closest asteriod for Rocket1 and Rocket2
    p.Rocket1.ClosestAsteriodDirection := '';
    p.Rocket2.ClosestAsteriodDirection := '';
    p.Rocket1.HasAsteriod := False;
    p.Rocket2.HasAsteriod := False;
    for i := 0 to Length(p.AsteriodList) - 1 do
    begin
      if (p.AsteriodList[i].XCoordinate = p.Rocket1.RocketCoordinate.XCoordinate) and
        (p.AsteriodList[i].YCoordinate = p.Rocket1.RocketCoordinate.YCoordinate) then
      begin
        p.Rocket1.HasAsteriod := True;
        p.Rocket1.ClosestAsteriodCoordinate := p.AsteriodList[i];
      end;
      if (p.AsteriodList[i].XCoordinate = p.Rocket2.RocketCoordinate.XCoordinate) and
        (p.AsteriodList[i].YCoordinate = p.Rocket2.RocketCoordinate.YCoordinate) then
      begin
        p.Rocket2.HasAsteriod := True;
        p.Rocket2.ClosestAsteriodCoordinate := p.AsteriodList[i];
      end;

      TempStr := GetPath(p.Rocket1.RocketCoordinate, p.AsteriodList[i]);
      if (Length(TempStr) < Length(p.Rocket1.ClosestAsteriodDirection)) or
        (p.Rocket1.HasAsteriod = False) then
      begin
        p.Rocket1.ClosestAsteriodDirection := TempStr;
        p.Rocket1.HasAsteriod := True;
        p.Rocket1.ClosestAsteriodCoordinate := p.AsteriodList[i];
      end;

      TempStr := GetPath(p.Rocket2.RocketCoordinate, p.AsteriodList[i]);
      if (Length(TempStr) < Length(p.Rocket2.ClosestAsteriodDirection)) or
        (p.Rocket2.HasAsteriod = False) then
      begin
        p.Rocket2.ClosestAsteriodDirection := TempStr;
        p.Rocket2.HasAsteriod := True;
        p.Rocket2.ClosestAsteriodCoordinate := p.AsteriodList[i];
      end;
    end;
    //sets directions to base
    p.Rocket1.BaseDirection :=
      GetPath(p.Rocket1.RocketCoordinate, p.Base.BaseCoordinate);
    p.Rocket2.BaseDirection :=
      GetPath(p.Rocket2.RocketCoordinate, p.Base.BaseCoordinate);

    //sets directions to friendly rocket
    p.Rocket1.FriendlyRocketDirection :=
      GetPath(p.Rocket1.RocketCoordinate, p.Rocket2.RocketCoordinate);
    p.Rocket2.FriendlyRocketDirection :=
      GetPath(p.Rocket2.RocketCoordinate, p.Rocket1.RocketCoordinate);

    //determine which one is closer
    p.Rocket1.IsCloserToAsteriod := False;
    p.Rocket2.IsCloserToAsteriod := False;
    if Length(p.Rocket1.ClosestAsteriodDirection) <=
      Length(p.Rocket2.ClosestAsteriodDirection) then
      p.Rocket1.IsCloserToAsteriod := True
    else
      p.Rocket2.IsCloserToAsteriod := True;
  end;

  procedure InitiateGamePlan;
  var
    PlayFiledText: TStringList;
    x, y: integer;
    CurrentLine: string;
  begin
    PlayFiledText := TStringList.Create;
    PlayFiledText.LoadFromFile(GAME_FILE_NAME);

    //reading X and Y dimenstions - index 1
    CurrentLine := PlayFiledText.Strings[1];
    XSize := GetFirstNumber(CurrentLine);
    YSize := GetFirstNumber(CurrentLine);

    //setting the game plan
    SetLength(GamePlan, XSize, YSize);

    //reading Player1 bases and rockets locations - index 2
    CurrentLine := Copy(PlayFiledText.Strings[2], 20, Length(PlayFiledText.Strings[2]));
    Player1.Base.BaseCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player1.Base.BaseCoordinate.YCoordinate := GetFirstNumber(CurrentLine);
    Player1.Rocket1.RocketCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player1.Rocket1.RocketCoordinate.YCoordinate := GetFirstNumber(CurrentLine);
    Player1.Rocket2.RocketCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player1.Rocket2.RocketCoordinate.YCoordinate := GetFirstNumber(CurrentLine);

    //reading Player1 bases and rockets locations - index 3
    CurrentLine := Copy(PlayFiledText.Strings[3], 20, Length(PlayFiledText.Strings[3]));
    Player2.Base.BaseCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player2.Base.BaseCoordinate.YCoordinate := GetFirstNumber(CurrentLine);
    Player2.Rocket1.RocketCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player2.Rocket1.RocketCoordinate.YCoordinate := GetFirstNumber(CurrentLine);
    Player2.Rocket2.RocketCoordinate.XCoordinate := GetFirstNumber(CurrentLine);
    Player2.Rocket2.RocketCoordinate.YCoordinate := GetFirstNumber(CurrentLine);

    //reading the game plan - starts at index 6
    for y := 0 to YSize - 1 do
      for x := 0 to XSize - 1 do
        GamePlan[x, y] := PlayFiledText.Strings[y + 6][x + 1];

    //who is who
    Player1.IsEnemy := False;
    Player2.IsEnemy := False;
    if Paramcount <> 1 then
      halt;
    if ParamStr(1) = '1' then
    begin
      Player2.IsEnemy := True;
      MyPlayer := Player1;
    end;
    if ParamStr(1) = '2' then
    begin
      Player1.IsEnemy := True;
      MyPlayer := Player2;
    end;

    //adding bases and rockets on the plan
    IndicateBaseAndRockets(Player1);
    IndicateBaseAndRockets(Player2);

    //adding targets
    LookForTargets(MyPlayer.Rocket1);
    LookForTargets(MyPlayer.Rocket2);
    GetAsteriodCoordinates(MyPlayer);
    PlayFiledText.Destroy;
  end;

  function RocketAtAsteriod(r: TRocket): boolean;
  begin
    Result := (r.HasAsteriod) and (r.ClosestAsteriodDirection = '');
  end;

  procedure OkNowIKnowEverythingAndNowIsTheTimeToGiveOrders;
  var
    TempStr: string;
  begin
    //    DebugListPlayer(MyPlayer);
    Command := '';
    //get both rockets on the closest asteriod
    //if there is danger, one defenses, the other shoots
    //drag to base
    if (MyPlayer.Rocket1.IsCloserToAsteriod) and
      (MyPlayer.Rocket1.ClosestAsteriodDirection <> '') then
      //Rocket1 is closer, but not at an asteriod yet
    begin
      AppendStr(Command, 'l ' + MyPlayer.Rocket1.ClosestAsteriodDirection[1]);
      if Length(MyPlayer.Rocket1.ClosestAsteriodDirection) > 1 then
        AppendStr(Command, ' ' + MyPlayer.Rocket1.ClosestAsteriodDirection[2]);
      TempStr := GetPath(MyPlayer.Rocket2.RocketCoordinate,
        MyPlayer.Rocket1.ClosestAsteriodCoordinate);
      AppendStr(Command, ':l ' + TempStr[1]);
      if Length(TempStr) > 1 then
        AppendStr(Command, ' ' + TempStr[2]);
    end;
    if (MyPlayer.Rocket2.IsCloserToAsteriod) and
      (MyPlayer.Rocket2.ClosestAsteriodDirection <> '') then
      //Rocket2 is closer, but not at an asteriod yet
    begin
      TempStr := GetPath(MyPlayer.Rocket1.RocketCoordinate,
        MyPlayer.Rocket2.ClosestAsteriodCoordinate);
      AppendStr(Command, 'l ' + TempStr[1]);
      if Length(TempStr) > 1 then
        AppendStr(Command, ' ' + TempStr[2]);
      AppendStr(Command, ':l ' + MyPlayer.Rocket2.ClosestAsteriodDirection[1]);
      if Length(MyPlayer.Rocket2.ClosestAsteriodDirection) > 1 then
        AppendStr(Command, ' ' + MyPlayer.Rocket2.ClosestAsteriodDirection[2]);
    end;
    if (MyPlayer.Rocket1.IsCloserToAsteriod) and
      (MyPlayer.Rocket1.ClosestAsteriodDirection = '') and not
      (RocketAtAsteriod(MyPlayer.Rocket2)) then
      //Rocket1 at asteriod, rocket2 follows
    begin
      AppendStr(Command, 'b');
      TempStr := GetPath(MyPlayer.Rocket2.RocketCoordinate,
        MyPlayer.Rocket1.ClosestAsteriodCoordinate);
      AppendStr(Command, ':l ' + TempStr[1]);
      if Length(TempStr) > 1 then
        AppendStr(Command, ' ' + TempStr[2]);
    end;
    if (MyPlayer.Rocket2.IsCloserToAsteriod) and
      (MyPlayer.Rocket2.ClosestAsteriodDirection = '') and not
      (RocketAtAsteriod(MyPlayer.Rocket1)) then
      //Rocket2 is at the asteriod, rocket1 follows
    begin
      TempStr := GetPath(MyPlayer.Rocket1.RocketCoordinate,
        MyPlayer.Rocket2.ClosestAsteriodCoordinate);
      AppendStr(Command, 'l ' + TempStr[1]);
      if Length(TempStr) > 1 then
        AppendStr(Command, ' ' + TempStr[2]);
      AppendStr(Command, ':b');
    end;
    if (RocketAtAsteriod(MyPlayer.Rocket1)) and (RocketAtAsteriod(MyPlayer.Rocket2)) then
      //both rockets are at asteriod
    begin
      if MyPlayer.Rocket1.ShootingDirection <> '' then
        //enemy is there = shield on + shoot back
        AppendStr(Command, 'b:s ' + MyPlayer.Rocket1.ShootingDirection[1])
      else
      begin
        //enemy is not there, drag the asteriod
        TempStr := 't ' + MyPlayer.Rocket1.BaseDirection[1];
        AppendStr(Command, TempStr + ':' + TempStr);
      end;
    end;
  end;

begin
  InitiateGamePlan;
  OkNowIKnowEverythingAndNowIsTheTimeToGiveOrders;
  Write(Command);
end.
