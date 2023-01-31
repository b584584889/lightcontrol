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
	<title>SMIMS VILS IoT - <?php echo $left_side_bar_footprint_tags;?></title>
</head>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_footprint_tags;?></h3>
		<div class="box-tools pull-right">
			<button type="button" class="btn btn-box-tool" onclick="open_footprinttags_page();return false;"><i class="fa fa-external-link"></i>
			</button>
			<!-- <a href="javascript:void(0)" onclick="open_footprint_page();return false;"><i class="fa fa-external-link"></i></a> -->
			<button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
			</button>
		</div>
	</div>
	<!-- /.box-header -->

	<div class="box-body no-padding">

		<div class="col-xs-12 col-sm-3 col-md-3 col-lg-3">

			<div class="box">

				<div class="box-body no-padding">

                    <div class="nav-tabs-custom">
                      <ul class="nav nav-tabs">
					  <?php
					  	// if ($_SESSION['type'] == 2)
						// {
						// 	echo "<li class='active'><a href='#loc_tab_users' data-toggle='tab'><span class='badge bg-aqua-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
						// 	echo "<li><a href='#loc_tab_sensors' data-toggle='tab'><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
						// 	echo "<li><a href='#loc_tab_search_name' data-toggle='tab'>搜尋名稱</a></li>";
						// }
						// else
						// {
							echo "<li class='active'><a href='#loc_tab_tags' data-toggle='tab'><span class='badge bg-blue-active'>"; echo $vilsdevices_tag; echo "</span></a></li>";
	                        echo "<li><a href='#loc_tab_locators' data-toggle='tab'><span class='badge bg-yellow-active'>"; echo $vilsdevices_locator; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_sensors' data-toggle='tab'><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_users' data-toggle='tab'><span class='badge bg-aqua-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
							echo "<li><a href='#loc_tab_search_name' data-toggle='tab'>搜尋名稱</a></li>";
						// }
         			  ?>
					  </ul>

                      <div class="tab-content">
                        <?php
						// if ($_SESSION['type'] == 2)
						// {
						// 	echo "<div class='tab-pane        table-responsive' id='loc_tab_tags'>";
						// }
						// else
						// {
							echo "<div class='tab-pane active table-responsive' id='loc_tab_tags'>";
						// }
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

                         <div class="tab-pane table-responsive" id="loc_tab_locators">
                             <div style="height:650px;overflow:scroll">
         					<table class="table table-hover">
         						<thead>
         							<tr style="background-color:#FFFFFF;">
         								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
         								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
         							</tr>
         						</thead>
         						<tbody id="locator_list_content">

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
					  // if ($_SESSION['type'] == 2)
					  // {
					  // 	echo "<div class='tab-pane active table-responsive' id='loc_tab_users'>";
					  // }
					  // else
					  // {
					  	echo "<div class='tab-pane        table-responsive' id='loc_tab_users'>";
					  // }
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

                       <div class="tab-pane table-responsive" id="loc_tab_search_name">
      					<table class="table table-hover">
      						<thead>
                                  <input type="text" id="loc_tab_search_text">
      						</thead>
      						<tbody id="search_list_content">

      						</tbody>
      					</table>
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

					<div id="record_box_content" style="background-color:rgb(236, 240, 245);">

                        <section class="content-header">
                          <h1>
                            標籤足跡
                            <span class="badge bg-green" id="record_box_title">Tag</span>
                          </h1>
                        </section>


                        <!-- Main content -->
                        <section class="content">

  						  <!-- row -->
                          <div class="row">
   						    <div class="col-md-6">

	  						  <ul class="pagination pagination-sm no-margin pull-left">
	                            <li><a href="javascript:;" onclick="move_daily_backward();return false;">&laquo</a></li>
	                            <li><a href="#" id="daily_targetdate"></a></li>
								<li><a href="javascript:;" onclick="move_daily_forward();return false;">&raquo</a></li>
                              </ul>
							  <input type='checkbox' id='show_target_date' onclick='click_show_target_date();return true;'></input>
  							  <label for='show_target_date'>選擇指定日期資料並輸出報表</label>
						    </div>
							<div class="col-md-3" style="display:none" id="daily_pdf_report_UI">
							  <a class="btn btn-success btn-xs" onclick='get_daily_pdf_report();return false;' id="daily_pdf_report_gen_btn">產生PDF報告</a>
							  <span class='pull-left' id="daily_pdf_report_status"></span>
							  <button type='button' class='btn btn-app' onclick='return false;' style="display:none" id="daily_pdf_report_dl_btn"><img src="/images/icons8-export-pdf-96.png" alt="Download report" title="Download report" height="42" width="42"></button>
                              <!-- <button type='button' class='btn btn-app pull-left' onclick='get_daily_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button> -->
							</div>

						  </div>
						  <!-- /.row -->

						  <!-- row -->
                          <div class="row">
   						    <div class="col-md-12">
								<p></br></p>
							</div>
						  </div>
						  <!-- /.row -->

                          <!-- row -->
                          <div class="row">
                            <div class="col-md-12">

                              <!-- The time line -->
                              <ul class="timeline" id="main_timeline">

                                <!-- END timeline item -->
                                <li>
                                  <i class="fa fa-clock-o bg-gray"></i>
                                </li>


                              </ul>
                              <!-- /.ul timeline-->
                            </div>
                            <!-- /.col -->
                          </div>
                          <!-- /.row -->

                        </section>
                        <!-- /.content -->

					</div>
					<!-- /.record_box_content -->
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

        var DEFINE_STRING_reload = "<?php echo $vilsfootprint_reload ?>";
        var DEFINE_STRING_nodata = "<?php echo $vilsfootprint_nodata ?>";
	</script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/footprinttags.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprinttags_src/00-daterangepicker.js"></script>
	<script src="/js/footprinttags_src/01-Leaflet.ImageOverlay.Rotated.js"></script>
	<script src="/js/footprinttags_src/02-simpleheat.js"></script>
	<script src="/js/footprinttags_src/03-HeatLayer.js"></script>
	<script src="/js/footprinttags_src/04-heatmap.js"></script>
	<script src="/js/footprinttags_src/05-bootstrap-datepicker.js"></script>
	<script src="/js/footprinttags_src/06-footprinttags.js"></script> -->


</html>
