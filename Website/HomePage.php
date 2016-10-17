<!DOCTYPE html>


<html>
<head>
        <link rel="stylesheet" href="style.css" type="text/css" media="screen" />
        <link rel="icon" href="website pictures/uct-logo.ico">
    </head>
    <?php
     include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
	$servername = "localhost";
		$username = "root";
		$password = "pimysql2016";
		$dbname = "UCTVendingMachine";

	// Create connection
	$conn = new mysqli($servername, $username, $password,$dbname);

	// Check connection
	if ($conn->connect_error) {
	    die("Connection failed: " . $conn->connect_error);
	} 

	$components = array();

	$sql = "SELECT * FROM Components";

	$args = $conn->query($sql);

	if (mysqli_num_rows($args) > 0) {
	    // output data of each row
	    while($row = mysqli_fetch_assoc($args)) {
	        array_push($components, $row["PartName"], $row["Empty"]);
	    }
	   
	}
	mysqli_close($conn);
	?>
	<body style="background-color:#CFCFCF;">
		
		<div id="left"></div>
		<div id="right"></div>
		<div id="top"></div>
		<div id="bottom"></div>
		<style type="text/css">#top, #bottom, #left, #right {
		background: #000000;
		position: fixed;
		}
		#left, #right {
			top: 0; bottom: 0;
			width: 15px;
			}
			#left { left: 0; }
			#right { right: 0; }
			
		#top, #bottom {
			left: 0; right: 0;
			height: 15px;
			}
			#top { top: 0; }
			#bottom { bottom: 0; }
		<!--[if lte IE 6]>
		<style>#top, #bottom, #left, #right { display: none; }</style>
		<![endif]--></style>
		 
		<style type="text/css">
		@media

		only screen and (max-width: 1000px),
		{
			#top, #bottom, #left, #right { display: none; }

			

			
		}

		@media
		only screen and (min-width: 1000px),
		{
			#mobilenote{
				display: none;
			}		
		}
		</style>

		<div id="mobilenote"><center><font face="Helvetica"><br>If you are viewing this site on a mobile device and the layout is not working please try using it in landscape mode. This site is not optimized for mobile viewing unfortunately. We apologize for the inconvenience.</font></center></div>

		<!--logo-->
		<center><img src="website pictures/WLVMLogo.png" height="auto" style="margin:20px 0 0 0; max-width:100% ;"></center>
		
		<!--navigationbar-->
		<style>
		ul {
		    list-style-type: none;
		    margin-top: 20px;
		    margin-bottom: 20px;
		    padding: 0;
		    overflow: hidden;
		    background-color: #333;
		    max-width: 1000px;
		}

		li {
		    float: left;
		}

		li a {
		    display: block;
		    color: white;
		    text-align: center;
		    padding: 14px 16px;
		    text-decoration: none;

		}

		li a:hover {
		    background-color: #111;
		}
		.active {
	    	background-color: #1E78B4;
		}
		</style>
		</head>
		<body>

		<center><ul>
		  <li><a class="active"><font face="Helvetica">Home</font></a></li>
		  <li><a href="Contact.php"><font face="Helvetica">Contact</font></a></li>
		  <li><a href="http://www.uct.ac.za/"><font face="Helvetica">UCT</font></a></li>
		</ul></center>

		<!--Page title and description-->
		<h1><b><center><font face="Helvetica">UCT White Lab Vending Machine</font></center></b></h1>
		
		<center><div style="max-width:1000px;"><p"><font face="Helvetica"><b>How to use:</b> Simply use the drop down boxes to select the quantity of components you want of each type(maximim of 
		 <?php 
			echo "<font face='Helvetica'>" . $complimit ."</font>";
		 ?>
		  ie the first
		 <?php 
			echo "<font face='Helvetica'>" . $complimit ."</font>";
		 ?>
		 you pick, the rest will be ignored). Once you have selected your components enter your student number in the field at the bottom of the page and hit submit(Double check it, if it is incorrect you will not get your components). Once sumbitted procceed to the Vending Machine in White lab and swipe your student card on the same day as ordering to collect your components. Note orders are reset at mindnight each day.</font></p></p"></div></center>

		<center><div style="max-width:1000px;"><p><center><font face="Helvetica">Please note you  are limited to 

		<?php 
		echo "<font face='Helvetica'>" . $partlimit ."</font>";
		?>

		components of the same type per order, and 

		<?php 
		echo "<font face='Helvetica'>" . $complimit ."</font>";
		?>

		 components in total per order every 24h.</font></center></p></div></center>
		 
		<hr>
		<!--Page title and description-->
		<h2><center><font face="Helvetica">Components:</font></center></h2>
		<hr>

		<form class="form" id="form1" action="request.php" method="post">

		<!--this is where the items to order will go-->
		<!--first line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">555 Timer</span><span style="margin-right: 260px;">LM311</span>LM393</font></center></p>

		<p><center><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\ne555.pdf"><font face = "Helvetica">Datasheet</font></a>

		<?php
			if (in_array('555', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == '555'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC1" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>	

		<span style="margin-right: 110px;"></span>		

		<a href="datasheets\lm311n.pdf"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM311', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM311'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC2" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 110px;"></span>		

		<a href="datasheets\LM393-D.PDF"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM393', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM393'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC3" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<p>&nbsp;</p>
		<!---->

		<!--second line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">LM741</span><span style="margin-right: 260px;">LM358</span>LM339</font></center></p>

		<p><center><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 14.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\LM741.pdf"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM741', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM741'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC4" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 110px;"></span>		

		<a href="datasheets\LM358-D.PDF"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM358', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM358'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC5" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 110px;"></span>		

		<a href="datasheets\LM339A.pdf"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM339', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM339'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC6" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<p>&nbsp;</p>
		<!---->

		<!--third line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">LM324</span><span style="margin-right: 260px;">SN7475</span>T74LS241B1</font></center></p>

		<p><center><img src="website pictures\dip 14.jpg" hspace="20"><img src="website pictures\dip 16.jpg" hspace="20"><img src="website pictures\dip 20.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\lm324-n.pdf"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('LM324', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'LM324'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC7" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 110px;"></span>		

		<a href="#null"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('unkown', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'unkown'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC8" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 110px;"></span>		

		<a href="#null"><font face = "Helvetica">Datasheet</font></a>
		
		<?php
			if (in_array('unkown', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'unkown'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> |quantity:</font>';
						    echo '<input type="number" name="IC9" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> |Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> |Out of Stock</font>';				
			}
		?>

		<p>&nbsp;</p>
		<!---->

		<!--third line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 200px;">DIP 8 IC Holder</span><span style="margin-right: 200px;">DIP 14 IC Holder</span>DIP 16 IC Holder</font></center></p>

		<p><center><img src="website pictures\dip 8 IC.jpg" hspace="20"><img src="website pictures\dip 14 IC.jpg" hspace="20"><img src="website pictures\dip 16 IC.jpg" hspace="20" ></center></p>

		<center>
		
		<?php
			if (in_array('DIP8HOLD', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'DIP8HOLD'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> quantity:</font>';
						    echo '<input type="number" name="IC10" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 225px;"></span>		
		
		<?php
			if (in_array('DIP14HOLD', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'DIP14HOLD'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica">quantity:</font>';
						    echo '<input type="number" name="IC11" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> Out of Stock</font>';				
			}
		?>

		<span style="margin-right: 225px;"></span>		
		
		<?php
			if (in_array('DIP16HOLD', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'DIP16HOLD'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> quantity:</font>';
						    echo '<input type="number" name="IC12" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> Out of Stock</font>';				
			}
		?>

		<p>&nbsp;</p>
		<!---->
		<!---->

		<!--third line-->
		<p><center><b><font face = "Helvetica">DIP 20 IC Holder</font></center></p>

		<p><center><img src="website pictures\dip 20 IC.jpg" hspace="20"></center></p>

		<center>
		
		<?php
			if (in_array('DIP20HOLD', $components)) {
				$count  = 0;
				while($count < sizeof($components)){
					if($components[$count] == 'DIP20HOLD'){
						if($components[$count+1] == 0){
							echo '<font face = "Helvetica"> quantity:</font>';
						    echo '<input type="number" name="IC13" max='.$partlimit. 'min="0" value="0" style="width:4em">';
						}else{
							echo '<font face = "Helvetica"> Out of Stock</font>';
						}
					}
					$count = $count + 2;
				}
			    
			} else {
				echo '<font face = "Helvetica"> Out of Stock</font>';				
			}
		?>


		<p>&nbsp;</p>
		
		      
		      <p class="student">
		        <input name="student" type="text" class="validate[required,custom[onlyLetter],length[0,100]] feedback-input" placeholder="*Student Number" id="student" required="" />
		      </p>		      
		      
		      <div class="submit">
		        <input type="submit" value="submit" id="button-blue"/>
		        <div class="ease"></div>
		      </div>
		    </form>

		<p><br><hr></p>
	</body>
</html>