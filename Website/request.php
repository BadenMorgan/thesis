<html>
	<body>
	<?php
		$servername = "localhost";
		$username = "root";
		$password = "pimysql2016";

		$conn = new mysqli($servername, $username, $password);

		// Check connection
		if ($conn->connect_error) {
		    die("Connection failed: " . $conn->connect_error);
		    echo "error connecting to database";
		}
		include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
		$student = $_POST['student'];
		function checkIC($partname,$quantity){
			if($quantity > 0){
				return $partname.": ".$quantity."\\n";
			}else{
				return '';
			}
		}
		$parts = checkIC('555 Timer',$_POST['IC1']).checkIC('LM311',$_POST['IC2']).checkIC('LM393',$_POST['IC3']).checkIC('LM741',$_POST['IC4']).checkIC('LM358',$_POST['IC5']).checkIC('LM339',$_POST['IC6']).checkIC('LM324',$_POST['IC7']).checkIC('unkown',$_POST['IC8']).checkIC('unkown',$_POST['IC9']).checkIC('L7805',$_POST['IC10']).checkIC('LM317',$_POST['IC11']);		
	?>
		<script type="text/javascript">
			alert('Parts Ordered for:\n' + '<?php echo $student; ?>' + '\nList of cart:\n' + '<?php echo $parts; ?>')
			//window.location.replace('HomePage.php')
		</script>
	</body>
</html>