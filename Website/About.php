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
		</style>

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
		  <li><a href="HomePage.php"><font face="Helvetica">Home</font></a></li>
		  <li><a href="Contact.php"><font face="Helvetica">Contact</font></a></li>
		  <li><a class="active" ><font face="Helvetica">About</font></a></li>
		  <li><a href="http://www.uct.ac.za/"><font face="Helvetica">UCT</font></a></
		</ul></center>

		<!--Page title and description-->
		<h1><b><center><font face="Helvetica">About the White Lab Vending Machine</font></center></b></h1>
		<p><center><font face="Helvetica">Pick The components you would like to order below and <br>proceed to the Vending machine within 12h to collect  </font></center></p>
		

		<!--this is where the items to order will go-->


		<p></p>
	</body>
</html>