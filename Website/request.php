<html>
	<body>
	<?php
		include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
		$servername = "localhost";
		$username = "root";
		$password = "pimysql2016";
		$dbname = "UCTVendingMachine";
		$counter = 0;

		$conn = new mysqli($servername, $username, $password,$dbname);

		// Check connection
		if ($conn->connect_error) {
		    die("Connection failed: " . $conn->connect_error);
		    echo "error connecting to database";
		}

		function checkIC($partname,$quantity){
			global $counter, $complimit;
			if($quantity > 0 && $counter < $complimit){
				$tempcount = $counter + $quantity;
				if($tempcount > $complimit){
					$quantity = $complimit - $counter;
					$counter = $complimit;
				}else{
					$counter = $tempcount;
				}
				if($quantity > 0){
					$randomno = mt_rand(1,65534);
					$sql = "INSERT INTO Orders VALUES ('" . $_POST['student'] ."', '". $partname ."', " . $quantity.", '".date('Y-m-d')."', 0,".$randomno.");";
					global $conn;
					if ( $conn->query($sql) === TRUE) {
						//echo "New record created successfully";
					} else {
						echo "Error: " . $sql . "<br>" .  $conn->error;
					}
					return $partname.": ".$quantity."\\n";
				}
			}else{
				return '';
			}
		}

		$parts = checkIC('555',$_POST['IC1']).checkIC('LM311',$_POST['IC2']).checkIC('LM393',$_POST['IC3']).checkIC('LM741',$_POST['IC4']).checkIC('LM358',$_POST['IC5']).checkIC('LM339',$_POST['IC6']).checkIC('LM324',$_POST['IC7']).checkIC('unkown',$_POST['IC8']).checkIC('unkown',$_POST['IC9']).checkIC('L7805',$_POST['IC10']).checkIC('LM317',$_POST['IC11']);		
	?>
		<script type="text/javascript">
			alert('Parts Ordered for:\n' + '<?php echo $_POST["student"]; ?>' + '\nList of cart:\n' + '<?php echo $parts; ?>')
			window.location.replace('HomePage.php')
		</script>
	</body>
</html>