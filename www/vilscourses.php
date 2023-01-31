<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">

    <link href="/css/vils.min.css" rel="stylesheet">
    <!-- fullCalendar -->
    <link rel='stylesheet' href='/css/fullcalendar/core/main.min.css' />
    <link rel='stylesheet' href='/css/fullcalendar/daygrid/main.min.css' />
    <link rel='stylesheet' href='/css/fullcalendar/timegrid/main.min.css' />
    <link rel='stylesheet' href='/css/fullcalendar/list/main.min.css' />

    <style>

      .popper,
      .tooltipNew {
        position: absolute;
        z-index: 9999;
        background: #FFC107;
        color: black;
        width: 150px;
        border-radius: 3px;
        box-shadow: 0 0 2px rgba(0,0,0,0.5);
        padding: 10px;
        text-align: center;
      }
      .style5 .tooltipNew {
        background: #1E252B;
        color: #FFFFFF;
        max-width: 200px;
        width: auto;
        font-size: .8rem;
        padding: .5em 1em;
      }
      .popper .popper__arrow,
      .tooltipNew .tooltipNew-arrow {
        width: 0;
        height: 0;
        border-style: solid;
        position: absolute;
        margin: 5px;
      }

      .tooltipNew .tooltipNew-arrow,
      .popper .popper__arrow {
        border-color: #FFC107;
      }
      .style5 .tooltipNew .tooltipNew-arrow {
        border-color: #1E252B;
      }
      .popper[x-placement^="top"],
      .tooltipNew[x-placement^="top"] {
        margin-bottom: 5px;
      }
      .popper[x-placement^="top"] .popper__arrow,
      .tooltipNew[x-placement^="top"] .tooltipNew-arrow {
        border-width: 5px 5px 0 5px;
        border-left-color: transparent;
        border-right-color: transparent;
        border-bottom-color: transparent;
        bottom: -5px;
        left: calc(50% - 5px);
        margin-top: 0;
        margin-bottom: 0;
      }
      .popper[x-placement^="bottom"],
      .tooltipNew[x-placement^="bottom"] {
        margin-top: 5px;
      }
      .tooltipNew[x-placement^="bottom"] .tooltipNew-arrow,
      .popper[x-placement^="bottom"] .popper__arrow {
        border-width: 0 5px 5px 5px;
        border-left-color: transparent;
        border-right-color: transparent;
        border-top-color: transparent;
        top: -5px;
        left: calc(50% - 5px);
        margin-top: 0;
        margin-bottom: 0;
      }
      .tooltipNew[x-placement^="right"],
      .popper[x-placement^="right"] {
        margin-left: 5px;
      }
      .popper[x-placement^="right"] .popper__arrow,
      .tooltipNew[x-placement^="right"] .tooltipNew-arrow {
        border-width: 5px 5px 5px 0;
        border-left-color: transparent;
        border-top-color: transparent;
        border-bottom-color: transparent;
        left: -5px;
        top: calc(50% - 5px);
        margin-left: 0;
        margin-right: 0;
      }
      .popper[x-placement^="left"],
      .tooltipNew[x-placement^="left"] {
        margin-right: 5px;
      }
      .popper[x-placement^="left"] .popper__arrow,
      .tooltipNew[x-placement^="left"] .tooltipNew-arrow {
        border-width: 5px 0 5px 5px;
        border-top-color: transparent;
        border-right-color: transparent;
        border-bottom-color: transparent;
        right: -5px;
        top: calc(50% - 5px);
        margin-left: 0;
        margin-right: 0;
      }

    </style>


    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - <?php echo $left_side_bar_courses_list; ?></title>
  </head>

<div class="wrapper">
<div class="content-wrapper" style="margin:0; padding:0; border:0 none;">
<section class="content" style="margin:0; padding:0; border:0 none;">

  <div class="col-md-12 col-sd-12">

      <div class="row">

          <div class="col-md-3">
            <div class="box box-solid" style="display:none">
              <div class="box-header with-border">
                <h4 class="box-title">活動</h4>
              </div>
              <div class="box-body">
                <!-- the events -->
                <div id="external-events">
                  <div class="external-event bg-green" id="111" style="display:none">單日活動</div>
                  <div class="external-event bg-yellow" id="222" style="display:none">每週活動</div>
                  <div class="external-event bg-aqua" id="333" style="display:none">每日活動重複5天</div>
                  <div class="external-event bg-light-blue" id="444" style="display:none">週六活動</div>
                  <div class="external-event bg-red" id="555" style="display:none">週日活動</div>
                  <div class="checkbox">
                    <label for="drop-remove" style="display:none">
                      <input type="checkbox" id="drop-remove">
                      remove after drop
                    </label>
                  </div>
                </div>

              </div>
              <!-- /.box-body -->
            </div>
            <!-- /. box -->

            <div class="box box-solid" style="display:block">
              <div class="box-header with-border">
                <h3 class="box-title">新增活動</h3>
              </div>
              <div class="box-body">
                  <div class="form-group">
                    <label>活動名稱</label>
                    <input type="text" id="course_title_InputText" value="新活動">
                  </div>
                  <div class="form-group">
                    <label>活動簡介</label>
                    <input type="text" id="course_description_InputText" value="簡介">
                  </div>
                  <div class="form-group">
                    <label>活動位置</label>
                    <div class="btn-group" id="area_list_dropdownUI"></div>
                  </div>

                  <div class="checkbox">
                    <label>
                      <input type="checkbox" id="course_allday_CheckBtn" onclick="course_allday_CheckBtn_onclick();">全天
                    </label>
                    <label>
                      <input type="checkbox" id="course_repeat_CheckBtn" onclick="course_repeat_CheckBtn_onclick();">重複
                    </label>
                  </div>

                  <div class="form-group" id="StartTimePicker">
                    <label>開始時間:</label>
                    <div class="input-group">
                      <input type="text" class="form-control timepicker" readonly="readonly" id="StartTimeInputText">

                      <div class="input-group-addon">
                        <i class="fa fa-clock-o"></i>
                      </div>
                    </div>
                    <!-- /.input group -->
                  </div>
                  <!-- /.form group -->


                    <div class="form-group" id="EndTimePicker">
                      <label>結束時間:</label>
                      <div class="input-group">
                        <input type="text" class="form-control timepicker" readonly="readonly" id="EndTimeInputText">
                        <div class="input-group-addon">
                          <i class="fa fa-clock-o"></i>
                        </div>
                      </div>
                      <!-- /.input group -->
                    </div>
                    <!-- /.form group -->

                    <!-- Date -->
                    <div class="form-group" id="DatePicker" style="display:none">
                      <label>選擇日期:</label>

                      <div class="input-group date">
                        <div class="input-group-addon">
                          <i class="fa fa-calendar"></i>
                        </div>
                        <input type="text" class="form-control pull-right" readonly="readonly" id="DatePickerInputText">
                      </div>
                      <!-- /.input group -->
                    </div>
                    <!-- /.form group -->

                  <!-- Date range -->
                  <div class="form-group" id="DateRangePicker" style="display:none">
                    <label>日期區間:</label>

                    <div class="input-group">
                      <div class="input-group-addon">
                        <i class="fa fa-calendar"></i>
                      </div>
                      <input type="text" class="form-control pull-right" readonly="readonly" id="DateRangeInputText">
                    </div>
                    <!-- /.input group -->
                  </div>
                  <!-- /.form group -->



                <div class="btn-group" style="width: 100%; margin-bottom: 10px;">
                  <!--<button type="button" id="color-chooser-btn" class="btn btn-info btn-block dropdown-toggle" data-toggle="dropdown">Color <span class="caret"></span></button>-->
                  <ul class="fc-color-picker" id="color-chooser">
                    <li><a class="text-aqua" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-blue" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-light-blue" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-teal" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-yellow" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-orange" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-green" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-lime" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-red" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-purple" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-fuchsia" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-muted" href="#"><i class="fa fa-square"></i></a></li>
                    <li><a class="text-navy" href="#"><i class="fa fa-square"></i></a></li>
                  </ul>
                </div>
                <!-- /btn-group -->

                <div class="form-group">
                  <button id="add-new-event" type="button" class="btn btn-primary btn-flat pull-right">新增</button>
                </div>
                <!-- /btn-group -->

                <!-- <div class="input-group">
                  <input id="new-event" type="text" class="form-control" placeholder="Event Title">
                  <div class="input-group-btn pull-right">
                    <button id="add-new-event" type="button" class="btn btn-primary btn-flat">新增</button>
                  </div>
                </div> -->

              </div>
            </div>


          </div>
          <!-- /.col -->

        <div class="col-md-9 col-sd-9">

          <!-- Courses -->
          <div class="box">
            <div class="box-header with-border">
              <h3 class="box-title"><?php echo $left_side_bar_courses_list; ?></h3>
              <div class="box-tools pull-right">
                  <button type="button" class="btn btn-box-tool" onclick="open_courses_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                  <!-- <a href="javascript:void(0)" onclick="open_sections_page();return false;"><i class="fa fa-external-link"></i></a> -->
              </div>
            </div>
            <!-- /.box-header -->

            <div class="box-body">
                <!-- THE CALENDAR -->
                <div id='calendar-container'>
                    <div id='calendar'></div>
                </div>
            </div>
            <!-- /.box-body -->

            </div>
            <!-- /.box-->


          </div>
          <!-- /.col -->


        </div>
        <!-- /.row -->

    </div>
    <!-- /.col -->

</section>
</div>
<!-- content-wrapper -->
</div>
<!-- ./wrapper -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
        var maingroups = "<?php echo $_SESSION["maingroups"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/courses.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/courses_src/fullcalendar/core/locales-all.min.js"></script>
    <script src="/js/courses_src/fullcalendar/core/main.js"></script>
    <script src="/js/courses_src/fullcalendar/interaction/main.js"></script>
    <script src="/js/courses_src/fullcalendar/daygrid/main.js"></script>
    <script src="/js/courses_src/fullcalendar/timegrid/main.js"></script>
    <script src="/js/courses_src/fullcalendar/list/main.js"></script>
    <script src="/js/courses_src/00-popper.js"></script>
    <script src="/js/courses_src/01-tooltip.js"></script>
    <script src="/js/courses_src/02-daterangepicker.js"></script>
    <script src="/js/courses_src/03-bootstrap-timepicker.js"></script>
    <script src="/js/courses_src/04-bootstrap-datepicker.js"></script>
    <script src="/js/courses_src/05-courses.js"></script>
    <script src="/js/courses_src/06-coursesapi.js"></script> -->

</html>
