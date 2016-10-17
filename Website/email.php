<?php
	include 'includes/variables.php';
	$fromHeader = "From: Vending Machine Report";
	$text = "the vending machine is jammed again.";
	mail($EmailAdmin,"Vending Machine Report" , "From: Vending Machine\n\n".$text."\n\n"."Sent: ".date('h:i:sa d-m-Y'),$fromHeader);
?>