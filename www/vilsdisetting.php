<?php
include ('checksession.php');
?>

<!DOCTYPE html>
<html lang="en">
	<head>
	  <meta charset="utf-8">
	  <meta http-equiv="X-UA-Compatible" content="IE=edge">
	  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
	  <link href="/css/vils.min.css" rel="stylesheet">
	  <meta name="description" content="SMIMS VILS IoT">
	  <meta name="author" content="SMIMS">
	  <title>SMIMS VILS IoT - DI Setting</title>
	</head>


	<div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsdisetting_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_disetting_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

    	<div class="box-body no-padding" style="background-color:rgb(236, 240, 245);">

			<div class="col-xs-12 col-sm-3 col-md-3 col-lg-3">
				<div class="box">
					<div class="box-body no-padding">

						<div class="nav-tabs-custom">
						  <ul class="nav nav-tabs">

							  <li class='active'><a href='#loc_tab_subgroup' data-toggle='tab'><span class='badge bg-green'><?php echo $vilsdisetting_subgroup; ?></span></a></li>

						  </ul>

						  <div class="tab-content">

							  <div class="tab-pane active" id="loc_tab_subgroup">
								  <div style="height:650px;overflow:scroll">
									 <table class="table table-hover">
										 <thead>
											 <tr style="background-color:#FFFFFF;">
												 <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
												 <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="subgroup_list_checkbox" onclick="subgroup_list_checkbox_click();"/></th>
												 <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdisetting_subgroup;?></th>
											 </tr>
										 </thead>
										 <tbody id="subgroup_list_content">

										 </tbody>
									 </table>
								  </div>
							  </div>
							   <!-- /.tab-pane -->

						  </div>
						  <!-- /.tab-content -->
				  	    </div>
					    <!-- nav-tabs-custom -->
					</div>
					<!-- /.box-body -->
				</div>
				<!-- /.box -->
			</div>
			<!-- col  -->

			<div class="col-xs-12 col-sm-9 col-md-9 col-lg-9">

				<div class="box">
					<div class="box-body no-padding">

						<table class="table table-hover">
							<thead>
								<tr style="background-color:#FFFFFF;">

									<th>
										<span>&nbsp;</span>
										<a class="btn btn-success pull-left" onclick="save_uisetting();" id="save_uisetting_button" style="display:none;">儲存目前設定</a>
									</th>
									<th>
										<div class="input-group margin">
											<div class="input-group-btn" id="copy_from_uisetting_dropdown" style="display:none;">
												<!-- <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown">選擇子集合複製到目前設定
													<span class="caret"></span>
													<span class="sr-only">Toggle Dropdown</span>
												</button>
												<ul class="dropdown-menu" role="menu" id="copy_from_uisetting_dropdown_menu">
												</ul>
												<button type="button" class="btn btn-default btn-sm" id="copy_from_uisetting_button" onclick="copy_from_uisetting_button_click();" style="visibility:hidden;">複製
												</button> -->
											</div>
										</div>
									</th>
									<th>
										<div class="input-group margin">
											<div class="input-group-btn" id="copy_to_uisetting_dropdown" style="display:none;">
												<button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown">複製目前設定到這些子集合
													<span class="caret"></span>
													<span class="sr-only">Toggle Dropdown</span>
												</button>
												<ul class="dropdown-menu" role="menu" id="copy_to_uisetting_dropdown_menu">
												</ul>
												<button type="button" class="btn btn-default btn-sm" id="copy_to_uisetting_button" onclick="copy_to_uisetting_button_click();" style="visibility:hidden;">複製
												</button>
											</div>
										</div>
									</th>
									<th>
										<span>&nbsp;</span>
										<a class="btn btn-danger pull-right" onclick="restore_uisetting();" id="restore_uisetting_button" style="display:none;">回復原始設定</a>
									</th>

								</tr>
							</thead>
							<tbody>
								<tr>
									<td colspan="4">
										<!-- Main content -->
									    <section class="content" id="main_content">

										</section>
								  	    <!-- /.section -->
									</td>
								</tr>
							</tbody>
						</table>

					</div>
					<!-- /.box-body -->
				</div>
				<!-- box  -->
			</div>
			<!-- /.col -->


		</div>
		<!-- /.box-body no-padding -->
	</div>
	<!-- /.box box-success -->
	<!-- /.MAP & BOX PANE -->

	<script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/disetting.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

	<script src="/js/disetting_src/00-disetting.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
