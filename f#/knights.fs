 

let boardsize = 5

// Setup board in 2d array setting all elements to true
//  ___________________
// |0,4|1,4|2,4|3,4|4,4| (x,y)
// |0,3|1,3|2,3|3,3|4,3|
// |0,2|1,2|2,2|3,2|4,2|
// |0,1|1,1|2,1|3,1|4,1|
// |0,0|1,0|2,0|3,0|4,0|
//  -------------------
//
let referenceboard = Array2D.init boardsize boardsize (fun x y -> true)

let legalmoves x y =
    // Generate every move (impossible or not)
    //
    let possibles = [(x - 1, y - 2); (x - 2, y - 1);
                     (x + 1, y - 2); (x + 2, y - 1);
                     (x + 1, y + 2); (x + 2, y + 1);
                     (x - 1, y + 2); (x - 2, y + 1);]
    
    // Prune the ones falling outside of the board
    //
    possibles |> List.filter (fun (x, y) -> x >= 0 && y >= 0 && x < boardsize && y < boardsize)

// Is a particular move still available to us? ie not already used
//
let availablemove x y (board : bool[,]) =
    board.[x,y] = true

let rec hunt startx starty (board : bool[,]) (history : List<(int*int)>) foundsolutionfunc = 
    // Mark the position we're at as being taken
    //
    board.[startx,starty] <- false

    // Check if we have a solution (ie we've covered the entire board)
    //
    if history.Length = boardsize * boardsize then
        foundsolutionfunc (history) |> ignore
    else
        // Calculate the set of available moves from this point, filtering by positions which we've already used
        //
        let availablemoves = legalmoves startx starty |> List.filter (fun (x,y) -> availablemove x y board)

        // For each available move, recurse
        //
        for (x,y) in availablemoves do
            hunt x y (Array2D.copy board) ((x,y) :: history) foundsolutionfunc

[<EntryPoint>]
let main args =
    
    let starttime = System.DateTime.Now

    let solutions = new System.Collections.Generic.List<System.Tuple<int,int>>()

    let startingpositions = [ for x in seq { 0 .. boardsize - 1 } do for y in seq { 0 .. boardsize - 1} do yield (x,y) ]

    Async.Parallel [ 
        for (x,y) in startingpositions -> 
            async { 
                printf "(%d,%d)" x y
                hunt x y (Array2D.copy referenceboard) [(x,y)] (fun solution ->
                    solutions.Add(new System.Tuple<int,int>(x,y))
                    printf "."
                ) 
            } 
    ] |> Async.RunSynchronously |> ignore

    let endtime = System.DateTime.Now

    printfn "\nFound %d solutions (took %03f seconds)" solutions.Count (endtime - starttime).TotalSeconds
    
    printfn "Enter to quit"
    System.Console.In.ReadLine() |> ignore

    0

