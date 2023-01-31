<?php
include ('checksession.php');
?>

<!DOCTYPE html>
<html lang="en">
	<?php
	include ('vi_html_head.php');
	?>


	<body style="background-color:rgb(236, 240, 245);" id="vi_page_body">


			<div class="modal fade" id="modal-qrcode">
			  <div class="modal-dialog">
				<div class="modal-content">
				  <div class="modal-header">
					<button type="button" class="close" data-dismiss="modal" aria-label="Close">
					  <span aria-hidden="true">&times;</span></button>
					<h4 class="modal-title">QR Code</h4>
				  </div>
				  <div class="modal-body">
					<div id='device_qrcode_type'></div>
					<div id='device_qrcode_name'></div>
					<div id='device_qrcode_macaddress'></div>
					<div id='device_qrcode_target'></div>
					<div id='device_qrcode_topic'></div>
					<div id='device_qrcode_datetime'></div>
					<img src='' alt='' class='margin'width='250' id='device_qrcode_ui'>
				  </div>
				  <div class="modal-footer">
				    <button type="button" class="btn btn-danger pull-left" onclick="confirm_reset_user_password();">重設密碼</button>
					<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
				  </div>
				</div>
				<!-- /.modal-content -->
			  </div>
			  <!-- /.modal-dialog -->
			</div>
			<!-- /.modal -->


		<!-- Main content -->
	    <section class="content" id="main_content">

		</section>
  	    <!-- /.section -->
		<div style="text-align:center">
			<button class="btn btn-default btn-flat" id="editblockbutton" onclick="editblockbutton_click();return false;" style="display:block;display:table;margin:0 auto;"><i class="fa fa-edit"></i>編輯顯示框</button></br>
			<button class="btn btn-default btn-flat" id="resettodefaultbutton" style="display:none;margin:0 auto;"><i class="fa fa-rotate-left"></i>重設顯示框</button></br>
			<button class="btn btn-default btn-flat" onclick="location.href='logout.php'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-power-off"></i>登出</button>
		</div>

	</body>

	<script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
	  var gl_userid = "<?php echo $_SESSION["userid"] ?>";
    </script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/di.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/di_src/00-di.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
