<!DOCTYPE html>


<html>
<head>
        <link rel="stylesheet" href="style.css" type="text/css" media="screen" />
    </head>
    <?php
     include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
	$servername = "localhost";
	$username = "root";
	$password = "pimysql2016";

	// Create connection
	$conn = new mysqli($servername, $username, $password);

	// Check connection
	if ($conn->connect_error) {
	    die("Connection failed: " . $conn->connect_error);
	} 
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
		  <li><a href="About.php"><font face="Helvetica">About</font></a></li>
		  <li><a href="http://www.uct.ac.za/"><font face="Helvetica">UCT</font></a></li>
		</ul></center>

		<!--Page title and description-->
		<h1><b><center><font face="Helvetica">UCT White Lab Vending Machine</font></center></b></h1>
		
		<p style="margin-right: 230px;"><center><font face="Helvetica"><b>How to use:</b> Simply use the drop down boxes to select the quantity of components you want of each type(maximim of 
		 <?php 
			echo "<font face='Helvetica'>" . $complimit ."</font>";
		 ?>
		  ie the first
		 <?php 
			echo "<font face='Helvetica'>" . $complimit ."</font>";
		 ?>
		 you pick, the rest will be ignored). Once you have selected your components enter your student number in the field at the bottom of the page and hit submit(Double check it, if it is incorrect you will not get your components). Once sumbitted procceed to the Vending Machine in White lab and swipe your student card within 24h to collect your components.</font></center></p>

		<p><center><font face="Helvetica">Please note you  are limited to 

		<?php 
		echo "<font face='Helvetica'>" . $partlimit ."</font>";
		?>
		 parts from each component every 24h.</font></center></p>
		 
		<hr>
		<!--Page title and description-->
		<h2><center><font face="Helvetica">Components:</font></center></h2>
		<hr>

		<!--this is where the items to order will go-->
		<!--first line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">555 Timer</span><span style="margin-right: 260px;">LM311</span>LM393</font></center></p>

		<p><center><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\ne555.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>
		
		<select name="IC1">		
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>
		<span style="margin-right: 130px;"></span>		

		<a href="datasheets\lm311n.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC2">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>

		<span style="margin-right: 130px;"></span>		

		<a href="datasheets\LM393-D.PDF"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC3">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select></center>

		<p>&nbsp;</p>
		<!---->

		<!--second line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">LM741</span><span style="margin-right: 260px;">LM358</span>LM339</font></center></p>

		<p><center><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 8.jpg" hspace="20"><img src="website pictures\dip 14.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\LM741.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC4">		
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>
		<span style="margin-right: 130px;"></span>		

		<a href="datasheets\LM358-D.PDF"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC5">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>

		<span style="margin-right: 130px;"></span>		

		<a href="datasheets\LM339A.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC6">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select></center>

		<p>&nbsp;</p>
		<!---->

		<!--third line-->
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">LM324</span><span style="margin-right: 260px;">unkown</span>unkown</font></center></p>

		<p><center><img src="website pictures\dip 14.jpg" hspace="20"><img src="website pictures\dip 16.jpg" hspace="20"><img src="website pictures\dip 20.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\lm324-n.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC7">		
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>
		<span style="margin-right: 130px;"></span>		

		<a href="#null"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC8">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>

		<span style="margin-right: 130px;"></span>		

		<a href="#null"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC9">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select></center>

		<p>&nbsp;</p>
		<!---->

		<!--fourth line-->		
		<p><center><b><font face = "Helvetica"><span style="margin-right: 260px;">L7805</span>LM317</font></center></p>

		<p><center><img src="website pictures\to220.jpg" hspace="20"><img src="website pictures\to220.jpg" hspace="20" ></center></p>

		<center>

		<a href="datasheets\L7805.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC10">		
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>
		<span style="margin-right: 130px;"></span>		

		<a href="datasheets\lm317.pdf"><font face = "Helvetica">Datasheet</font></a>
		<font face = "Helvetica"> |quantity:</font>

		<select name="IC11">
		    <?php for ($i = 0; $i <= $partlimit; $i++) : ?>
		        <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
		    <?php endfor; ?>
		</select>

		<p>&nbsp;</p>
		<!---->

		<form class="form" id="form1">
		      
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