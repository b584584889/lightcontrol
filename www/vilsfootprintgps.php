<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">

<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
	<link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/images/favicon.ico">
	<link href="/css/vils.min.css" rel="stylesheet">
	<meta name="description" content="SMIMS VILS IoT">
	<meta name="author" content="SMIMS">
	<title>SMIMS VILS IoT - <?php echo $left_side_bar_footprint_GPS;?></title>
</head>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_footprint_GPS;?></h3>
		<div class="box-tools pull-right">
			<button type="button" class="btn btn-box-tool" onclick="open_footprintgps_page();return false;"><i class="fa fa-external-link"></i>
			</button>
			<!-- <a href="javascript:void(0)" onclick="open_footprint_page();return false;"><i class="fa fa-external-link"></i></a> -->
			<button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
			</button>
		</div>
	</div>
	<!-- /.box-header -->

	<div class="box-body no-padding">

		<div class="col-xs-12 col-sm-2 col-md-2 col-lg-2">

			<div class="box">

				<div class="box-body no-padding">

					<div class="nav-tabs-custom">
                      <ul class="nav nav-tabs">
					  <?php
					  	if ($_SESSION['type'] == 2)
						{
							echo "<li class='active'><a href='#loc_tab_users' data-toggle='tab'><span class='badge bg-aqua-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_sensors' data-toggle='tab'><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
						}
						else
						{
							echo "<li class='active'><a href='#loc_tab_tags' data-toggle='tab'><span class='badge bg-blue-active'>"; echo $vilsdevices_tag; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_sensors' data-toggle='tab'><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_users' data-toggle='tab'><span class='badge bg-aqua-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
						}
						?>
					  </ul>

                      <div class="tab-content">

						  <?php
						  if ($_SESSION['type'] == 2)
						  {
						  	echo "<div class='tab-pane        table-responsive' id='loc_tab_tags'>";
						  }
						  else
						  {
						  	echo "<div class='tab-pane active table-responsive' id='loc_tab_tags'>";
						  }
						  ?>
                            <div style="height:650px;overflow:scroll">
        					<table class="table table-hover">
        						<thead>
        							<tr style="background-color:#FFFFFF;">
        								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
        								<th style="width:60px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
        							</tr>
        						</thead>
        						<tbody id="tag_list_content">

        						</tbody>
        					</table>
                            </div>
                        </div>
                         <!-- /.tab-pane -->

					  <div class="tab-pane table-responsive" id="loc_tab_sensors">

                          <div class="box">
                              <div class="box-body no-padding">
                                <table class="table table-bordered">
                                  <thead>
                                    <tr style="background-color:#cce6ff;">
                                      <th style="width: 10px">#</th>
                                      <th><?php echo $vilssensordevice_name;?></th>
                                    </tr>
                                  </thead>
                                  <tbody id="sensor_list_content">
                                  </tbody>
                                </table>
                            </div>
                            <!-- /.box-body -->
                          </div>
                          <!-- /.box -->

                      </div>
                      <!-- /.tab-pane -->

					  <?php
					  if ($_SESSION['type'] == 2)
					  {
					    echo "<div class='tab-pane active table-responsive' id='loc_tab_users'>";
					  }
					  else
					  {
					    echo "<div class='tab-pane        table-responsive' id='loc_tab_users'>";
					  }
					  ?>
                          <div style="height:650px;overflow:scroll">
      					<table class="table table-hover">
      						<thead>
      							<tr style="background-color:#FFFFFF;">
  									<th style="width: 10px">#</th>
									<th><?php echo $vilspushnotification_username;?></th>
      							</tr>
      						</thead>
      						<tbody id="user_list_content">

      						</tbody>
      					</table>
                          </div>
                      </div>
                       <!-- /.tab-pane -->

                       </div>
                       <!-- /.tab-content -->
                    </div>
                    <!-- nav-tabs-custom -->


                    <!-- <div style="height:650px;overflow:scroll">
					<table class="table table-hover">
						<thead>
							<tr style="background-color:#FFFFFF;">
								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">Tag</th>
							</tr>
						</thead>
						<tbody id="tag_list_content">
						</tbody>
					</table>
                    </div> -->

				</div>
				<!-- /.box-body -->
			</div>
			<!-- /.box -->
		</div>
		<!-- col  -->

		<div class="col-xs-12 col-sm-10 col-md-10 col-lg-10">

			<div class="box">
				<div class="box-header with-border">
					<h3 class="box-title" id="record_box_title"></h3>
				</div>
				<!-- /.box-header -->
				<div class="box-body no-padding">

					<div id="record_box_content">

					</div>
					<!-- /.record_box_content -->
				</div>
				<!-- /.box-body -->
                <div class="box-footer clearfix">
                  <ul class="pagination pagination-sm no-margin pull-right">
                    <li><a href="javascript:void(0)" onclick="record_box_move_page_forward();return false;">&laquo;</a></li>
                    <li><a href="javascript:void(0)" id="record_box_move_page_index">1</a></li>
                    <li><a href="javascript:void(0)" onclick="record_box_move_page_backward();return false;">&raquo;</a></li>
                  </ul>
                </div>
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
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
		var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
	</script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/footprintgps.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprintgps_src/00-daterangepicker.js"></script>
    <script src="/js/footprintgps_src/01-footprintgps.js"></script> -->

</html>
