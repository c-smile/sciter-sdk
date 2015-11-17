<?php 

  // start jsonrpc
  include("jsonrpc-start.php");
  
  // our web service functions
  
  function add($p1,$p2)
  {
    return $p1 + $p2;
  }
  function sub($p1,$p2)
  {
    return $p1 - $p2;
  }
  
  // declare them callable outside
  external_method("add", "sub");
  
  // end jsonrpc, return result
  include("jsonrpc-end.php");
?>



