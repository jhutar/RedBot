open System
open System.IO

let filename = "playfield.txt"

type PlayerData = 
    {
        BaseLocation : int*int 
        Ship1Location : int*int 
        Ship2Location : int*int 
        Points : int 
    }

type Asteroid = 
    {
        Value : int 
        Coords : int*int 
    }

type GameData = 
    {
      Size : int*int
      Player1Data : PlayerData
      Player2Data : PlayerData
      MyPlayerNumber : int
      Asteroids : Asteroid list   
    }

type StaticStrategyInfo = 
    {
        OurShipCoords : (int*int) list
        EnemyShipCoords : (int*int) list
        AllShipCoords : (int*int) list
        Size1Asteroids : Asteroid list
        Size2Asteroids : Asteroid list
        FreeAsteroids : Asteroid list
        OccupiedAsteroids : Asteroid list
        OccupiedAsteroidsInFirePosition : Asteroid list
        G : GameData
        Myself : PlayerData
        Opponent : PlayerData
    }

type ShipMovementStrategyInfo =
    {
        nearestAsteroid : Asteroid
        nearestAsteroidShip2OtherThanShip1 : Asteroid option
        SInfo : StaticStrategyInfo
    }

let inline (|-) list predicate = List.filter predicate list

let parsePlayerNumber commandLineArgs =
    match commandLineArgs with
    | [|a|] -> int(a)
    | _ -> failwith("Wrong player number specified")

let parsePlayerData (playerData:string) =
    let l data = List.ofSeq(data)
    let EatTill separator (from:string list) = 
            List.ofSeq(from.Head.Split([|separator|],2)).Tail @ from.Tail
    let TakeTill separator (from:string list) = 
        let splitted = List.ofSeq(from.Head.Split([|separator|],2)) 
        splitted.Tail @ [splitted.Head] @ from.Tail
    let data = [playerData] |> EatTill ':' |> EatTill ':' |> TakeTill ' ' |> EatTill '[' |> TakeTill ',' |> TakeTill ']' 
               |> EatTill '[' |> TakeTill ',' |> TakeTill ']' 
               |> EatTill '[' |> TakeTill ',' |> TakeTill ']' |> List.rev
    match data with 
    | points::baseX::baseY::ship1LocX::ship1LocY::ship2LocX::ship2LocY::_ ->
        { BaseLocation=(int(baseX),int(baseY))
          Ship1Location=(int(ship1LocX),int(ship1LocY))
          Ship2Location=(int(ship2LocX),int(ship2LocY))
          Points = int(points)}
    | _ -> failwith("Wrong format of player data " + playerData)

let parseAsteroids gameField = 
    let rec parse rowNum gameField =
        let rec parseRowOfAsteroids rowNum colNum gameRow = 
            match gameRow with
            | []->[]
            | ' '::tail -> parseRowOfAsteroids rowNum (colNum+1) tail
            | a::tail -> { Value=int(a.ToString());Coords=(colNum,rowNum)}::parseRowOfAsteroids rowNum (colNum+1) tail
        match gameField with 
        | []   -> []
        | h::t -> parseRowOfAsteroids rowNum 0 h @ parse (rowNum+1) t
    parse 0 gameField

let initiateGame playerNumber (lines:string list) = 
    let parseWidthAndHeight (widthAndHeightString:string) = 
        let fields = List.ofSeq(widthAndHeightString.Split(' '))
        match fields with 
        | [width;height] -> (int(width),int(height))
        | _ -> failwith("Wrong format of line 2")
        
    match lines with 
    |_::widthAndHeightString::player1Data::player2Data::_::_::playField ->
        let widthAndHeight = parseWidthAndHeight widthAndHeightString
        {Size = widthAndHeight
         Player1Data = parsePlayerData player1Data
         Player2Data = parsePlayerData player2Data
         MyPlayerNumber = playerNumber
         Asteroids = parseAsteroids(List.map(fun x-> List.ofSeq(x)) playField)
         }
    | _ -> failwith("Wrong format of file")

//end parsing
//begin ai
let IsInPlay (x,y) playfield =
    x>=0 && y >=0 && x < (playfield.Size |> fst) && y < (playfield.Size |> snd)

let distance (x1:int, y1:int) (x2:int, y2:int) =
    (Math.Abs (x1 - x2)) + (Math.Abs (y1 - y2))

let allowedMove (xcoord,ycoord) playField direction = 
    match direction with
    //can be 2,0;1,0;1,1;0,1;0,2.
    | (x:int,y:int) when ( ( x<>0 || y<>0) && Math.Abs(x) + Math.Abs(y) <=2 ) -> IsInPlay (xcoord + x,ycoord + y) playField
    | _ -> false

let allowedMoveWithAsteroid bothShipsOnSamePlace (xcoord:int,ycoord:int) (gameData:GameData) direction = 
    let asteroidsOnPosition = gameData.Asteroids |> List.filter (fun (ast) -> fst ast.Coords=xcoord && snd ast.Coords=ycoord) 
    match asteroidsOnPosition with
    | [] -> false
    | [a] when a.Value = 1 -> Math.Abs xcoord + Math.Abs ycoord = 1 && allowedMove (xcoord,ycoord) gameData direction
    | [a] when a.Value = 2 -> Math.Abs xcoord + Math.Abs ycoord = 1 && allowedMove (xcoord,ycoord) gameData direction && bothShipsOnSamePlace
    | _ -> failwith "More asteroids on one place aren't permitted"


let cartesian xs ys = 
    xs |> List.collect (fun x -> ys |> List.map (fun y -> x, y))

let random = new System.Random()

let safeFieldMatrix gameData = 
    let opponent = 
        if gameData.MyPlayerNumber = 1 
        then gameData.Player2Data 
        else gameData.Player1Data 
    let allFields = cartesian [0..fst gameData.Size] [0..snd gameData.Size]
    if random.Next(2) = 0 
    then 
        let rec safeFields fields = 
            match fields with 
            | [] -> [] 
            | coords::t when coords = opponent.Ship1Location 
                            || coords = opponent.Ship2Location -> coords::safeFields t 
            | (x,y)::t when (x = fst (opponent.Ship1Location) 
                            || y = snd (opponent.Ship1Location)) 
                            && ((x,y) <> opponent.Ship1Location) -> safeFields t 
            | (x,y)::t when (x = fst (opponent.Ship2Location) 
                            || y = snd (opponent.Ship2Location)) 
                            && ((x,y) <> opponent.Ship2Location) -> safeFields t 
            | fields::t -> fields::safeFields t 
        safeFields allFields 
    else 
        let rec safeFields fields =
            match fields with 
            | [] -> [] 
            | (x,y)::t when (x = fst (opponent.Ship1Location) 
                            || y = snd (opponent.Ship1Location)) -> safeFields t 
            | (x,y)::t when (x = fst (opponent.Ship2Location) 
                            || y = snd (opponent.Ship2Location)) -> safeFields t 
            | fields::t -> fields::safeFields t 
        safeFields allFields

let moveTo shipCoords (xe:int, ye:int) = 
    let distX = Math.Abs(fst shipCoords - xe)
    let distance = distX + Math.Abs(snd shipCoords - ye)
    if random.Next(distance) <= distX then
        match shipCoords with
        | (x,y) when x > xe -> (" l", (x - 1, y))
        | (x,y) when x < xe -> (" p", (x + 1, y))
        | (x,y) when y > ye -> (" n", (x, y - 1))
        | (x,y) when y < ye -> (" d", (x, y + 1))
        | _ -> ("", shipCoords)
    else
        match shipCoords with
        | (x,y) when y > ye -> (" n", (x, y - 1))
        | (x,y) when y < ye -> (" d", (x, y + 1))
        | (x,y) when x > xe -> (" l", (x - 1, y))
        | (x,y) when x < xe -> (" p", (x + 1, y))
        | _ -> ("", shipCoords)

// letani
let fly fromPos toPos = 
    let fstpos = moveTo fromPos toPos
    let finalpos = moveTo (snd fstpos) toPos
    (sprintf "l%s%s" (fst fstpos) (fst finalpos), snd finalpos)

// tahnuti asteroidu
let drag fromPos toPos =
    let finalpos = moveTo fromPos toPos
    (sprintf "t%s" (fst finalpos), snd finalpos)

// obrana
let defense =
    "b"

// utok
let attack fromPos toPos =
    let finalpos = moveTo fromPos toPos
    (sprintf "s%s" (fst finalpos), snd finalpos)

let rec elem el els =
    match els with
    | e::tail when e = el -> true
    | _::tail -> elem el tail
    | _ -> false

let rec remove el els =
    match els with
    | e::tail when el = e -> tail
    | e::tail -> e::(remove el tail) 
    | _ -> []

let rec removeAsteroidOnCoords (coords:int*int) (asteroids:Asteroid list) =
    match asteroids with
    | e::tail when coords = e.Coords -> tail
    | e::tail -> e::(removeAsteroidOnCoords coords tail) 
    | _ -> []

// vraci (true/false, opponentCoords)
let canShootOnOpponentWithAsteroid shipCoords myOpponent asteroids =
    let opponentAsteroids =
        List.filter (fun el -> ((distance el.Coords myOpponent.Ship1Location) < 2) || ((distance el.Coords myOpponent.Ship2Location) < 2))
                     asteroids
    let sameRowOrColumn (x, y) (u,v) =
        (x = u) || (y = v)
    let rec shootable asteroids =
        match asteroids with
        | a::tail when (sameRowOrColumn myOpponent.Ship1Location shipCoords) 
                        && ((distance a.Coords myOpponent.Ship1Location) < 2)  -> (true, myOpponent.Ship1Location)
        | a::tail when (sameRowOrColumn myOpponent.Ship2Location shipCoords) 
                        && ((distance a.Coords myOpponent.Ship2Location) < 2)  -> (true, myOpponent.Ship2Location)
        | _::tail -> shootable tail
        | _ -> (false, (0,0))
    shootable opponentAsteroids

// vraci (true/false, opponentCoords)
let canShootOnOpponent shipCoords myOpponent =
    let sameRowOrColumn (x, y) (u,v) =
        (x = u) || (y = v)
    if shipCoords <> myOpponent.Ship1Location && sameRowOrColumn shipCoords myOpponent.Ship1Location 
    then (true, myOpponent.Ship1Location)
    else if shipCoords <> myOpponent.Ship2Location && sameRowOrColumn shipCoords myOpponent.Ship2Location
    then (true, myOpponent.Ship2Location)
    else (false, (0,0))


let isAsteroidOnPosition asteroids pos =
    let rec isThere ast =
        match ast with
        | a::tail when 0 = (distance a.Coords pos) -> true
        | _::tail -> isThere tail
        | _ -> false
    isThere asteroids

let getFreeAsteroids gameData = 
    let opponent = 
        if gameData.MyPlayerNumber = 1 
        then gameData.Player2Data
        else gameData.Player1Data
    List.filter (fun el -> (el.Coords <> opponent.Ship1Location) && (el.Coords <> opponent.Ship2Location)) gameData.Asteroids

let getAsteroidsOfValue asteroids value =
    List.filter (fun el -> el.Value = value) asteroids

let getNearestAsteroid asteroids shipCoords =
    let nearest = 
        match asteroids with
        | [] -> failwith "getNearestAsteroid: empty list"  
        | _  -> (List.sortBy (fun el -> distance shipCoords el.Coords) asteroids)
    let nearestDistance = distance shipCoords nearest.Head.Coords
    let allNearest = 
        match asteroids with
        | [] -> failwith "getNearestAsteroid: empty list"  
        | _  -> Array.ofList(List.filter (fun shortest -> distance shipCoords shortest.Coords = nearestDistance) asteroids)
    allNearest.[random.Next(allNearest.Length)]

let getPlayersTuple gameData =
    match gameData.MyPlayerNumber with
    | 1 -> (gameData.Player1Data, gameData.Player2Data)
    | 2 -> (gameData.Player2Data, gameData.Player1Data)
    | _ -> failwith "Incorrect player number"

let getStaticStrategyInfo gameData:StaticStrategyInfo = 
    let (myself, opponent) = getPlayersTuple gameData
    let ourShips = [myself.Ship1Location;myself.Ship2Location]
    let enemyShips = [opponent.Ship1Location;opponent.Ship2Location]
    let allShips = List.append ourShips enemyShips
    let occupiedAsteroids =  gameData.Asteroids |- fun el -> (el.Coords = opponent.Ship1Location) || (el.Coords = opponent.Ship2Location)
    let inLineOfFire (x1,y1) (x2,y2) = x1 = x2 || y1 = y2
    { 
    Size1Asteroids = gameData.Asteroids |- fun x-> x.Value = 1
    Size2Asteroids = gameData.Asteroids |- fun x-> x.Value = 2
    FreeAsteroids =  gameData.Asteroids |- fun el -> (el.Coords <> opponent.Ship1Location) && (el.Coords <> opponent.Ship2Location)
    OccupiedAsteroids = occupiedAsteroids
    OccupiedAsteroidsInFirePosition = occupiedAsteroids |- fun x -> inLineOfFire x.Coords myself.Ship1Location || inLineOfFire x.Coords myself.Ship2Location
    OurShipCoords = ourShips
    EnemyShipCoords = enemyShips
    AllShipCoords = allShips
    G = gameData
    Myself = myself
    Opponent = opponent
    }

let inline (|?) a b =
    if a=[] then b else a

// vyhravame - obe lode strili na nepritele
let defenseStrategy (gameData:GameData) = 
    let s = getStaticStrategyInfo gameData
    let (t1, t2) =
        (getNearestAsteroid gameData.Asteroids s.Myself.Ship1Location, getNearestAsteroid gameData.Asteroids s.Myself.Ship2Location)
    let (target1, target2) = 
        if t1.Value = 2
        then (t1, t1)
        else 
            if t2.Value = 2
            then (t2, t2)
            else (t1, t2)

    let fstshipMove = 
        match (canShootOnOpponent s.Myself.Ship1Location s.Opponent) with
        | (true, coords) when random.Next(10) <= 7 -> attack s.Myself.Ship1Location coords
        | _ ->  if (distance s.Myself.Ship1Location target1.Coords = 0)
                then
                    let dragmove1 = drag s.Myself.Ship1Location s.Myself.BaseLocation
                    match target1.Value with
                    | 2 when (distance s.Myself.Ship2Location target1.Coords) <> 0 -> (defense, s.Myself.Ship1Location)
                    | _ when (isAsteroidOnPosition gameData.Asteroids (snd dragmove1)) -> fly s.Myself.Ship1Location (snd dragmove1)
                    | _ -> dragmove1
                else fly s.Myself.Ship1Location target1.Coords
    let sndshipMove = 
        match (canShootOnOpponent s.Myself.Ship2Location s.Opponent) with
        | (true, coords) when coords <> (snd fstshipMove) && random.Next(10) <= 7 -> attack s.Myself.Ship2Location coords
        | _ ->  if (distance s.Myself.Ship2Location target2.Coords = 0)
                then
                    let dragmove2 = drag s.Myself.Ship2Location s.Myself.BaseLocation
                    match target2.Value with
                    | 2 when (distance s.Myself.Ship1Location target2.Coords) <> 0 -> (defense, s.Myself.Ship2Location)
                    | 2 -> fstshipMove
                    | 1 when (snd fstshipMove) = (snd dragmove2) -> (defense, s.Myself.Ship2Location)
                    | _ when (isAsteroidOnPosition gameData.Asteroids (snd dragmove2)) -> fly s.Myself.Ship2Location (snd dragmove2)
                    | _ -> dragmove2
                else fly s.Myself.Ship2Location target2.Coords
    sprintf "%s:%s" (fst fstshipMove) (fst sndshipMove)

// prohravame - obe lode jdou tahat asteroidy
let attackStrategy (gameData:GameData) =
    let (myself, opponent) = 
        match gameData.MyPlayerNumber with
        | 1 -> (gameData.Player1Data, gameData.Player2Data)
        | 2 -> (gameData.Player2Data, gameData.Player1Data)
        | _ -> failwith "Incorrect player number"
    let asteroids = getFreeAsteroids gameData
    match asteroids with
    | [] -> defenseStrategy gameData
    | _ ->
        let trgt = getNearestAsteroid asteroids myself.Ship1Location
        let target = if trgt.Value = 2 || ((remove trgt asteroids) = []) || (distance trgt.Coords myself.Ship1Location) <= (distance trgt.Coords myself.Ship2Location)
                     then trgt
                     else getNearestAsteroid (remove trgt asteroids) myself.Ship1Location
        let fstshipMove =
            if (distance myself.Ship1Location target.Coords) = 0
            then
                let dragmove1 = drag myself.Ship1Location myself.BaseLocation
                match target.Value with
                | 2 when (distance myself.Ship2Location target.Coords) <> 0 -> (defense, myself.Ship1Location)
                | _ when (isAsteroidOnPosition gameData.Asteroids (snd dragmove1)) -> fly myself.Ship1Location (snd dragmove1)
                | _ -> dragmove1
            else fly myself.Ship1Location target.Coords
        let sndshipMove =
            let removed = remove target asteroids |? asteroids 
            let sndtarget = if target.Value = 2 then target else (getNearestAsteroid removed myself.Ship2Location)
            if (distance myself.Ship2Location sndtarget.Coords) = 0
            then 
                let dragmove2 = drag myself.Ship2Location myself.BaseLocation
                match sndtarget.Value with
                | 2 when (distance myself.Ship1Location sndtarget.Coords) <> 0 -> (defense, myself.Ship2Location)
                | 2 -> fstshipMove // tahnou spolecne 
                | 1 when (snd fstshipMove) = (snd dragmove2) -> (defense, myself.Ship2Location)
                | _ when (isAsteroidOnPosition gameData.Asteroids (snd dragmove2)) -> fly myself.Ship2Location (snd dragmove2)
                | _ -> dragmove2
            else fly myself.Ship2Location sndtarget.Coords
        sprintf "%s:%s" (fst fstshipMove) (fst sndshipMove)

let chooseStrategy (gameData:GameData) =
    let player1Score = gameData.Player1Data.Points
    let player2Score = gameData.Player2Data.Points
    let rec total asteroids n =
        match asteroids with
        | a::tail -> total tail (a.Value + n)
        | _ -> n
    let totalScore = total gameData.Asteroids (player1Score + player2Score)
    let strategyByScore score1 score2 =
        match score1 - score2 with
        | x when x <= 2 && score2 > 2 -> defenseStrategy
        | _ when random.Next(100) < 90 -> attackStrategy
        | _ -> defenseStrategy
    match gameData.MyPlayerNumber with
    | 1 -> strategyByScore player1Score player2Score
    | _ -> strategyByScore player2Score player1Score 

let startGame (gameData:GameData) = 
    let possibleMoves = cartesian [-2..2] [-2..2] 
    let possibleMovesWithAsteroid = cartesian [-1..1] [-1..1] 
    let (myPlayerData, opponent) = 
        match gameData.MyPlayerNumber with
        | 1 -> (gameData.Player1Data, gameData.Player2Data)
        | 2 -> (gameData.Player2Data, gameData.Player1Data)
        | _ -> failwith "Incorrect player number"
    let combineCoords = fun (x1,y1) (x2,y2) -> (x1+x2,y1+y2)
    let legalMovesShip1 = possibleMoves |> List.filter (allowedMove myPlayerData.Ship1Location gameData) |> List.map (combineCoords myPlayerData.Ship1Location)
    let legalMovesShip2 = possibleMoves |> List.filter (allowedMove myPlayerData.Ship2Location gameData) |> List.map (combineCoords myPlayerData.Ship2Location)
    let bothShipsOnSamePlace = (myPlayerData.Ship1Location = myPlayerData.Ship2Location)
    let legalMovesWithAsteroidShip1 = possibleMovesWithAsteroid |> List.filter (allowedMoveWithAsteroid bothShipsOnSamePlace myPlayerData.Ship1Location gameData)  |> List.map (combineCoords myPlayerData.Ship1Location)
    let legalMovesWithAsteroidShip2 = possibleMovesWithAsteroid |> List.filter (allowedMoveWithAsteroid bothShipsOnSamePlace myPlayerData.Ship2Location gameData)  |> List.map (combineCoords myPlayerData.Ship2Location)

    gameData |> chooseStrategy gameData |> printfn "%s"
    ()

[<EntryPoint>]
let main argv = 
    let playerNumber = parsePlayerNumber argv
    let lines = List.ofSeq(File.ReadAllLines(filename))
    let game = initiateGame playerNumber lines
    let returnString = game |> startGame
    //ignore(Console.ReadLine())
    0 // return an integer exit code
