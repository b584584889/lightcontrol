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
	</div>
	<!-- /.box-header -->

	<div class="box-body no-padding">

		<div class="col-xs-12 col-sm-12 col-md-12 col-lg-12">

			<div class="box">
				<div class="box-body no-padding">

					<div id="record_box_content" style="background-color:rgb(236, 240, 245);">

                        <section class="content-header">
                          <h1>
                            GPS 軌跡記錄
                            <span class="badge bg-green" id="record_box_title">Tag</span>
                          </h1>

                        </section>


                        <!-- Main content -->
                        <!-- <section class="content"> -->

  						  <!-- row -->
                          <!-- <div class="row"> -->
						  	<!-- <ul class="timeline" id="main_timeline"> </ul> -->
							<ul id="main_timeline"> </ul>
						  <!-- </div> -->
						  <!-- /.row -->

						  <!-- row -->
                          <!-- <div class="row">
   						    <div class="col-md-12">
								<p></br></p>
							</div>
						  </div> -->
						  <!-- /.row -->

                          <!-- row -->
                          <div class="row">
                            <div class="col-md-12">
                              <div id="record_box_heatmap_gps" style="height:80vh;"></div>

                            </div>
                            <!-- /.col -->
                          </div>
                          <!-- /.row -->

                        <!-- </section> -->
                        <!-- /.content -->

					</div>
					<!-- /.record_box_content -->
				</div>
				<!-- /.box-body -->
			</div>
			<!-- box  -->
		</div>
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

        var query_date = "<?php echo $date ?>";
        var query_type = "<?php echo $type ?>";
		var query_nodeid = "<?php echo $nodeid ?>";
        var query_macaddress = "<?php echo $macaddress ?>";

		// console.log("query_date:"+query_date);
		// console.log("query_type:"+query_type);
		// console.log("query_nodeid:"+query_nodeid);
		// console.log("query_macaddress:"+query_macaddress);

	</script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/footprinttaggps.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprinttaggps_src/00-Leaflet.ImageOverlay.Rotated.js"></script>
	<script src="/js/footprinttaggps_src/01-simpleheat.js"></script>
	<script src="/js/footprinttaggps_src/02-HeatLayer.js"></script>
	<script src="/js/footprinttaggps_src/03-heatmap.js"></script>
	<script src="/js/footprinttaggps_src/04-footprinttaggps.js"></script> -->


</html>
