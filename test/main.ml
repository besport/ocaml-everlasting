let debug fmt = Printf.ksprintf print_endline fmt

let time f () = 
  let t1 = Unix.gettimeofday () in 
  f () ; 
  let t2 = Unix.gettimeofday () in 
  t2 -. t1 

 
let test5 () = 
  let eternal = Eternal.create 200000 in 
  Eternal.set eternal 0 [ 1; 2; 3] ; 
  let i = Eternal.get eternal 0 in
  List.iter print_int i

type u = { uids : int list ; visible : bool } 


let test6 () = 
  let v = { uids = [ 1; 2; 4]; visible = false }  in 
  let c = Eternal.copy v in
  let u = Eternal.update c { uids = [ 4; 2; 1] ; visible = true } in 
  debug "> %B %B" v.visible u.visible
  

let _ = 
  test6 () ;  
  debug "> collected, exiting" 
    
  
