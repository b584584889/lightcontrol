<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
	<?php
	include ('vi_html_head.php');
	?>

<!-- MAP & BOX PANE -->
<div class="box box-success">
	<div class="box-header with-border">
		<h3 class="box-title"><?php echo $left_side_bar_footprint_courses;?></h3>
	</div>
	<!-- /.box-header -->

	<div class="box-body no-padding">

		<div class="col-xs-12 col-sm-5 col-md-5 col-lg-5">

			<div class="box">

				<div class="box-body no-padding">

                    <div class="nav-tabs-custom">
                      <ul class="nav nav-tabs">
                        <li class="active"><a href="#tab_course_list" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilsfootprint_courses_list;?></span></a></li>
						<li><a href="#tab_search_name" data-toggle="tab">搜尋名稱</a></li>
					  </ul>


                    <div class="tab-content">

                      <div class="tab-pane active table-responsive" id="tab_course_list">
                        <div style="height:650px;overflow:scroll">
     					  <table class="table table-hover">
						    <thead>
							  <tr style="background-color:#FFFFFF;">
								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
								<th style="width:200px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsfootprint_courses_name;?></th>
								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsfootprint_courses_description;?></th>
								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsfootprint_courses_datetime;?></th>
							  </tr>
						    </thead>
						    <tbody id="course_list_content">
					        </tbody>
					      </table>
                        </div>
                       </div>
					   <!-- /.tab-pane -->

					   <div class="tab-pane table-responsive" id="tab_search_name">
					    <table class="table table-hover">
					   	 <thead>
					   		   <input type="text" id="tab_search_text">
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

		<div class="col-xs-12 col-sm-7 col-md-7 col-lg-7">

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
   						    <div class="col-md-6">

	  						  <ul class="pagination pagination-sm no-margin pull-left">
	                            <li><a href="javascript:;" onclick="move_daily_forward();return false;">&laquo</a></li>
	                            <li><a href="#" id="daily_targetdate"></a></li>
								<li><a href="javascript:;" onclick="move_daily_backward();return false;">&raquo</a></li>
                              </ul>
							  <input type='checkbox' id='show_target_date' onclick='click_show_target_date();return true;'></input>
  							  <label for='show_target_date'>只顯示指定日期資料</label>
						    </div>
							<div class="col-md-3" style="display:none" id="course_pdf_report_UI">
							  <span class='pull-left' id="course_pdf_report_status"></span>
  	                          <button type='button' class='btn btn-app pull-left' onclick='get_course_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>
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


<div style="text-align:center">
    <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
</div>

	<script>
		var project_id = 1;

        var DEFINE_STRING_reload = "<?php echo $vilsfootprint_reload ?>";
        var DEFINE_STRING_nodata = "<?php echo $vilsfootprint_nodata ?>";
	</script>

	<script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
	<script src="/js/footprintcourses.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

	<!-- <script src="/js/footprintcourses_src/00-daterangepicker.js"></script>
	<script src="/js/footprintcourses_src/01-bootstrap-datepicker.js"></script>
    <script src="/js/footprintcourses_src/02-footprintcourses.js"></script> -->

</html>
