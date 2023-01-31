<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

<div class="wrapper">
<div class="content-wrapper" style="margin:0; padding:0; border:0 none;">
<section class="content" style="margin:0; padding:0; border:0 none;">

  <div class="col-md-12 col-sd-12">

      <div class="row">

        <div class="col-md-9 col-sd-9">

          <!-- Courses -->
          <div class="box">
            <div class="box-header with-border">
              <h3 class="box-title"><?php echo $left_side_bar_courses_list; ?></h3>
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


<div style="text-align:center">
    <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
</div>

    <script>
        var project_id = 1;
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
