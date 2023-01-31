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
	<title>SMIMS VILS IoT - <?php echo $left_side_bar_footprint_inoutarea;?></title>
</head>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_footprint_inoutarea;?></h3>
		<div class="box-tools pull-right">
			<button type="button" class="btn btn-box-tool" onclick="open_footprintinout_page();return false;"><i class="fa fa-external-link"></i>
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

                    <div style="height:650px;overflow:scroll">
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
                    </div>
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
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
	</script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/footprintinout.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprintinout_src/00-daterangepicker.js"></script>
    <script src="/js/footprintinout_src/01-footprintinout.js"></script> -->

</html>
