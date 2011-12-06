let debug fmt = Printf.ksprintf print_endline fmt


type u = 
    {
      cnt : int ; 
      label : string ;
    }


let create () = 
  { cnt = Random.int 1000  ; label = "coucou" } 

let test1 () = 
  Random.self_init () ; 
  debug "> eternal"  ; 
  let t = Eternal.create 10 in  
  Gc.full_major (); 
  Eternal.set t 0 (create ()) ;
  Gc.full_major (); 
  let e = Eternal.unsafe_get t 0 in 
  Gc.full_major (); 
  debug "> found elt %d" e.cnt  ;    
  
      
  debug "> store created" ; 
  Gc.full_major (); 
  debug "> collected" 

let _ = 
  ignore (test1 ()); 
  Gc.full_major (); 
  debug "> collected, exiting" 
    
  
