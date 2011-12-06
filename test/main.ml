let debug fmt = Printf.ksprintf print_endline fmt


type u = 
    {
      cnt : int ; 
      label : string ;
    }

let _ = 
  Random.self_init () 

let create () = 
  { cnt = Random.int 1000  ; label = "coucou" } 

(*
let test1 () = 
  Random.self_init () ; 
  debug "> eternal"  ; 
  
  let t = Eternal.create 10 in  
  
  Gc.full_major (); 
  debug "> set elt" ; 
  let u = (create ()) in
  Eternal.set t 0 u ; 
  Gc.full_major (); 
  
  let e = Eternal.unsafe_get t 0 in 
  debug "> found elt %d" e.cnt  ;    
  
  let e = Eternal.unsafe_get t 0 in 
      
  debug "> store created" ; 
  Gc.full_major (); 
  debug "> collected" 
*)

(*
let test2 () = 
  debug "about to create an eternal reference" ; 
  let a = Eternal.t1 (create ()) in 
  debug "it's created, now accessing it"; 
  debug "> created element %d" (!a).cnt 
*)


let time f () = 
  let t1 = Unix.gettimeofday () in 
  f () ; 
  let t2 = Unix.gettimeofday () in 
  t2 -. t1 

type t = { cnt : string ; bval : int ; visible : bool } 
let create () =  { cnt = "coucou" ; bval = 1; visible = false } 


let print_stats () = 
  let open Gc in 
      let stats = Gc.stat () in
      debug "minor_words %f live_words %d" stats.minor_words stats.live_words

let test3 n = 
  let e = ref [] in
  let _ = 
    for i = 0 to n do
      let c =  Eternal.copy (create ()) in 
      e := c :: !e 
    done in 
    print_stats () ;
    Gc.full_major () ; 
    print_stats () ;
    let t = time Gc.full_major () in
    print_stats () ;
    debug "gc took %f seconds" t 


type i  = float array  
let test4 () = 
  let i = [| 0.0 ; 4.12; 12.21 |] in
  print_stats () ;
  let j = Eternal.copy i in
  Gc.full_major () ;
  Gc.full_major () ;
  print_stats () ;
  debug "> %d %f" (Array.length j) j.(2)

let _ = 
  test4 () ;  
  debug "> collected, exiting" 
    
  
