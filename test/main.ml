let debug = Printf.ksprintf print_endline 

let _ = 
  debug "> eternal"  ; 
  ignore(Eternal.create ())  ;
  debug "> store created" ; 
  Gc.full_major (); 
  debug "> collected" 
