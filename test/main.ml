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

let test3 n = 
  let e = ref [] in
  for i = 0 to n do
    let c = Eternal.copy (create ()) in 
    e := c :: !e 
  done ; 
  Gc.full_major () ; 
  let t = time Gc.full_major () in 
  debug "gc took %f seconds" t 

let _ = 
  ignore (test3 100000); 
  ignore (test3 1000000); 
  debug "> collected, exiting" 
    
  
