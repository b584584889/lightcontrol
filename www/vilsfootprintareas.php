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
	<title>SMIMS VILS IoT - <?php echo $left_side_bar_footprint_areas;?></title>
</head>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_footprint_areas;?></h3>
		<div class="box-tools pull-right">
			<button type="button" class="btn btn-box-tool" onclick="open_footprintareas_page();return false;"><i class="fa fa-external-link"></i>
			</button>
			<button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
			</button>
		</div>
	</div>
	<!-- /.box-header -->

	<div class="box-body no-padding">

		<div class="col-xs-12 col-sm-3 col-md-3 col-lg-3">

			<div class="box">

				<div class="box-body no-padding">

                    <div style="height:650px;overflow:scroll">
					<table class="table table-hover">
						<thead>
							<tr style="background-color:#FFFFFF;">
								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsfootprint_areas_name;?></th>
								<th style="width:100px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsfootprint_areas_type;?></th>
							</tr>
						</thead>
						<tbody id="area_list_content">
						</tbody>
					</table>
                    </div>
				</div>
				<!-- /.box-body -->
			</div>
			<!-- /.box -->
		</div>
		<!-- col  -->

		<div class="col-xs-12 col-sm-9 col-md-9 col-lg-9">

			<div class="box">
				<div class="box-header with-border">
					<h3 class="box-title" id="record_box_title"></h3>
				</div>
				<!-- /.box-header -->
				<div class="box-body no-padding">

					<div id="record_box_content">


                        <section class="content-header">
                          <h1>
                            Timeline
                            <span class="badge bg-green" id="record_box_title"></span>
                          </h1>
                        </section>

                        <!-- Main content -->
                        <section class="content">

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
	<script src="/js/footprintareas.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprintareas_src/00-daterangepicker.js"></script>
	<script src="/js/footprintareas_src/01-footprintareas.js"></script> -->

</html>
