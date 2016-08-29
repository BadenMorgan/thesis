<html>
	<body>
	<?php
		include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
		$fromHeader = "From: ". $_POST['email'] . " (" . $_POST['student'] .")";
		mail($EmailAdmin,"Vending Machine Comment" , $_POST['text'],$fromHeader);
	?>
		<script type="text/javascript">
			alert('Email sent')
			window.location.replace('Contact.php')
		</script>
	</body>
</html>