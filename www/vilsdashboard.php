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
	<title>SMIMS VILS IoT - <?php echo $left_side_bar_dashboard_list;?></title>
</head>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_dashboard_list;?></h3>
		<div class="box-tools pull-right">
			<button type="button" class="btn btn-box-tool" onclick="open_dashboard_page();return false;"><i class="fa fa-external-link"></i>
			</button>
			<button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
			</button>
		</div>
	</div>
	<!-- /.box-header -->

	<div class="box-body">

      <!-- Filter -->
      <div class="row">
        <section class="col-lg-12">
          <div class="box box-solid">
            <div class="box-header">
              <table style="width:100%">
                <tr>
                  <td>
                      <span class="badge bg-light-blue" style="display:none">過濾條件<i class="fa fa-plus"></i></span>
                      <span class="label label-info" style="font-size:15px;display:none">2019年7月29日<a href="javascript:;"><i class="fa fa-times-circle" style="font-size:15px;color:rgb(236, 240, 245);"></i></a></span>
                  </td>
                  <td style="width:60px">統計範圍</td>
                  <td style="width:280px">
                      <ul class="pagination pagination-sm no-margin pull-right">
                        <!-- <li><a href="javascript:;">&laquo;</a></li> -->
                        <li><a href="javascript:;" id="data_range_datetime">2019/07/28 - 2019/07/29</a></li>
                        <!-- <li><a href="javascript:;">&raquo;</a></li> -->
                      </ul>
                  </td>
                </tr>
              </table>
            </div>
          </div>
          <!-- /.sbox -->
        </section>
        <!-- /.section -->
      </div>
      <!-- /.row -->

      <div class="row">
        <section class="col-xs-6 col-sm-4 col-md-4 col-lg-4 connectedSortable">
		<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
			<div class="box" style="background-color:rgb(236, 240, 245);">
                <div class="box-header">
                    過去30分鐘活動用戶數
                </div>
				<div class="box-body no-padding">
					<div>
                        <table>
                          <tr>
                            <td style="width:20px"></td>
                            <td>
                                <h1 id="main_activity_user_count">0</h1>
                            </td>
                          </tr>

                          <tr>
                            <td style="width:20px"></td>
                            <td>
                                <p class="text-muted">每分鐘活動用戶數</p>
                                <div class="description-block">
                                  <div class="sparkbar pad" data-color="#744" id="main_activity_alive_count">0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0</div>
                                  <!-- <p class="text-muted" align="left">目前</p> -->
                                </div>
                            </td>
                          </tr>

                        </table>
					</div>
					<!-- /.record_box_content -->
				</div>
				<!-- /.box-body -->
			</div>
			<!-- box  -->
		<!-- </div> -->
        </section>
		<!-- /.col -->


        <section class="col-xs-6 col-sm-4 col-md-4 col-lg-4 connectedSortable">
		<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
          <!-- TABLE: LATEST ORDERS -->
          <div class="box box-info" style="background-color:rgb(236, 240, 245);">
              <div class="box-header" id="areauser_box_header">
                  區域內活動用戶數
              </div>
              <div class="box-body no-padding">
                <div>
                    <div class="table-responsive">
                      <table class="table no-margin">
                        <thead>
                        <tr>
                          <th>區域名稱</th>
                          <th>人數</th>
                          <th>平均時間</th>
                        </tr>
                        </thead>
                        <tbody id="areauser_box_body">
                        </tbody>
                      </table>
                    </div>
                    <div class="box-footer" style="background-color:rgb(236, 240, 245);">
                      <a href="javascript:void(0)" class="pull-right" style="display:none">查看詳情<i class="fa fa-arrow-circle-right"></i></a>
                    </div>
                    <!-- /.table-responsive -->
                </div>
            </div>
            <!-- /.box-body -->
          </div>
          <!-- /.box -->
		<!-- </div> -->
        </section>
		<!-- /.col -->

        <section class="col-xs-6 col-sm-4 col-md-4 col-lg-4 connectedSortable">
		<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
          <!-- TABLE: LATEST ORDERS -->
          <div class="box box-info" style="background-color:rgb(236, 240, 245);">
              <div class="box-header">
                  功能使用活動用戶數
              </div>
              <div class="box-body no-padding">
                <div>
                    <div class="table-responsive">
                      <table class="table no-margin">
                        <thead>
                        <tr>
                          <th>功能名稱</th>
                          <th>人數</th>
                          <th>平均時間</th>
                        </tr>
                        </thead>
                        <tbody id="operationmode_box_body">
                        </tbody>
                      </table>
                    </div>
                    <!-- /.table-responsive -->
                </div>
            </div>
            <!-- /.box-body -->
          </div>
          <!-- /.box -->
		<!-- </div> -->
        </section>
		<!-- /.col -->
    </div>
    <!-- /.row -->


    <div class="row">

			<section class="col-xs-6 col-sm-6 col-md-4 col-lg-4 connectedSortable">
			<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
	          <!-- TABLE: LATEST ORDERS -->
	          <div class="box box-info" style="background-color:rgb(236, 240, 245);">
	              <div class="box-header">
	                  緊急事件用戶
	              </div>
	              <div class="box-body no-padding">
	                <div>
	                    <div class="table-responsive">
	                      <table class="table no-margin">
	                        <thead>
	                        <tr>
	                          <th>用戶名</th>
	                          <th>發生次數</th>
	                          <th>最近發生時間</th>
	                        </tr>
	                        </thead>
	                        <tbody id="sos_box_body">
	                        </tbody>
	                      </table>
	                    </div>
	                    <div class="box-footer" style="background-color:rgb(236, 240, 245);">
	                        <a href="javascript:void(0)" class="pull-right" style="display:none">查看詳情<i class="fa fa-arrow-circle-right"></i></a>
	                    </div>
	                    <!-- /.table-responsive -->
	                </div>
	            </div>
	            <!-- /.box-body -->
	          </div>
	          <!-- /.box -->
			<!-- </div> -->
	        </section>
			<!-- /.col -->


	        <section class="col-xs-6 col-sm-6 col-md-6 col-lg-6 connectedSortable">
			<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
	          <!-- TABLE: LATEST ORDERS -->
	          <div class="box box-info" style="background-color:rgb(236, 240, 245);">
	              <div class="box-header" id="nodemoveoffset_box_header">
	                  用戶總和移動距離
	              </div>
	              <div class="box-body no-padding">
	                <div>
	                    <div class="table-responsive">
	                      <table class="table no-margin">
	                        <thead>
	                        <tr>
	                          <th>用戶名稱</th>
	                          <th>距離(公尺)</th>
	                          <th>移動時間</th>
	                        </tr>
	                        </thead>
	                        <tbody id="nodemoveoffset_box_body">
	                        </tbody>
	                      </table>
	                    </div>
	                    <div class="box-footer" style="background-color:rgb(236, 240, 245);">
	                      <a href="javascript:void(0)" class="pull-right" style="display:none">查看詳情<i class="fa fa-arrow-circle-right"></i></a>
	                    </div>
	                    <!-- /.table-responsive -->
	                </div>
	            </div>
	            <!-- /.box-body -->
	          </div>
	          <!-- /.box -->
			<!-- </div> -->
	        </section>
			<!-- /.col -->
	</div>
    <!-- /.row -->
    <div class="row">

            <section class="col-xs-6 col-sm-6 col-md-4 col-lg-4 connectedSortable">
              <!-- TABLE: LATEST ORDERS -->
              <div class="box box-info" style="background-color:rgb(236, 240, 245);">
                  <div class="box-header">
                      設備運行狀況 - 目前
                  </div>
                  <div class="box-body no-padding">
                    <div>
                        <div class="table-responsive">
                          <table class="table no-margin">
                            <thead>
                            <tr>
                              <th>設備種類</th>
                              <th>總數</th>
                              <th>在線數</th>
                              <th>離線數</th>
                            </tr>
                            </thead>
                            <tbody id="device_alive_box_body">
                            </tbody>
                          </table>
                        </div>
                        <div class="box-footer" style="background-color:rgb(236, 240, 245);">
                            <a href="javascript:void(0)" class="pull-right" style="display:none">查看詳情<i class="fa fa-arrow-circle-right"></i></a>
                        </div>
                        <!-- /.table-responsive -->
                    </div>
                </div>
                <!-- /.box-body -->
              </div>
              <!-- /.box -->
            </section>
    		<!-- /.section -->

            <section class="col-xs-6 col-sm-6 col-md-4 col-lg-4 connectedSortable">

              <!-- TABLE: LATEST ORDERS -->
              <div class="box box-info" style="background-color:rgb(236, 240, 245);">
                  <div class="box-header">
                      系統服務狀況 - 目前
                  </div>
                  <div class="box-body no-padding">
                    <div>
                        <div class="table-responsive">
                          <table class="table no-margin">
                            <thead>
                            <tr>
                              <th>系統服務總數</th>
                              <th>正常運作數</th>
                              <th>非正常運作數</th>
                            </tr>
                            </thead>
                            <tbody id="system_service_box_body">
                            </tbody>
                          </table>
                        </div>
                        <!-- /.table-responsive -->
                    </div>
                </div>
                <!-- /.box-body -->
              </div>
              <!-- /.box -->
            </section>
    		<!-- /.section -->

			<section class="col-xs-6 col-sm-6 col-md-4 col-lg-4 connectedSortable">

			  <!-- TABLE: LATEST ORDERS -->
			  <div class="box box-info" style="background-color:rgb(236, 240, 245);">
				  <div class="box-header">
					  系統資訊 - 目前
				  </div>
				  <div class="box-body no-padding">
					<div>
						<div class="table-responsive">
						  <table class="table no-margin">
							<thead>
							<tr>
							  <th>系統項目</th>
							  <th>數據</th>
							</tr>
							</thead>
							<tbody id="systeminfo_box_body">
							</tbody>
						  </table>
						</div>
						<!-- /.table-responsive -->
					</div>
				</div>
				<!-- /.box-body -->
			  </div>
			  <!-- /.box -->
			</section>
			<!-- /.section -->



        <section class="col-xs-12 col-sm-8 col-md-6 col-lg-6 connectedSortable">
		<!-- <div class="col-xs-6 col-sm-3 col-md-3 col-lg-3"> -->
          <!-- TABLE: LATEST ORDERS -->
          <div class="box box-info" style="background-color:rgb(236, 240, 245);">
              <div class="box-header">
                  巡檢事件
              </div>
              <div class="box-body no-padding">
                <div>
                    <div class="table-responsive">
                      <table class="table no-margin">
                        <thead>
                        <tr>
                          <th>巡檢路線</th>
                          <th>事件</th>
                          <th>發生日期</th>
                          <th>發生時間</th>
                        </tr>
                        </thead>
                        <tbody id="inspection_box_body">
                        </tbody>
                      </table>
                    </div>
                    <div class="box-footer" style="background-color:rgb(236, 240, 245);">
                        <a href="javascript:void(0)" class="pull-right" style="display:none">查看詳情<i class="fa fa-arrow-circle-right"></i></a>
                    </div>
                    <!-- /.table-responsive -->
                </div>
            </div>
            <!-- /.box-body -->
          </div>
          <!-- /.box -->
        </section>
		<!-- /.section -->
    </div>
    <!-- /.row -->

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
	<script src="/js/dashboard.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/dashboard_src/00-dashboard.js"></script> -->


    <script>

        // -----------------
        // - SPARKLINE BAR -
        // -----------------
        $('.sparkbar').each(function () {
          var $this = $(this);
          $this.sparkline('html', {
            type    : 'bar',
            height  : $this.data('height') ? $this.data('height') : '30',
            barColor: $this.data('color'),
            tooltipFormat: "{{value}}",
            tooltipValueLookups: {
                    levels: $.range_map({ ':-1': '分鐘前'})
                }
          });
        });


        // Make the dashboard widgets sortable Using jquery UI
        $('.connectedSortable').sortable({
          placeholder         : 'sort-highlight',
          connectWith         : '.connectedSortable',
          handle              : '.box-header, .nav-tabs',
          forcePlaceholderSize: true,
          zIndex              : 999999
        });
        $('.connectedSortable .box-header, .connectedSortable .nav-tabs-custom').css('cursor', 'move');

        // jQuery UI sortable for the todo list
        $('.todo-list').sortable({
          placeholder         : 'sort-highlight',
          handle              : '.handle',
          forcePlaceholderSize: true,
          zIndex              : 999999
        });

	</script>

</html>
