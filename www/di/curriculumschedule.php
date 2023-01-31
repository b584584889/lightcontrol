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

    <div class="modal fade" id="modal-record_detail">
      <div class="modal-dialog">
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-label="Close">
              <span aria-hidden="true">&times;</span></button>
            <h4 class="modal-title">課程詳細資訊</h4>
          </div>
          <div class="modal-body">
            <div id='record_detail_name'></div>
            <div id='record_detail_type'></div>
            <div id='record_detail_time'></div>
            <div id='record_detail_signintime'></div>
            <div id='record_detail_totalhours'></div>
            <div id='record_detail_location'></div>
            <div id='record_detail_description'></div>
            <div id='record_detail_teacher'></div>
            <div id='record_detail_cost'></div>
            <div id='record_detail_note'></div>

          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
          </div>
        </div>
        <!-- /.modal-content -->
      </div>
      <!-- /.modal-dialog -->
    </div>
    <!-- /.modal -->


 <div class="row">

  <div class="col-md-12 col-sd-12">

      <!-- weekschedule -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $curriculumschedule; ?></h3>
          <div class="box-tools pull-right">
          </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body">

        <table class="table table-bordered" style="border-color:black;border:3px #111111 solid;">
            <thead>
              <tr style="background-color:#acdafb;border-color:black;">
                <th style="text-align:center;border-color:black;" colspan="6" id="schedule_title">

                    <ul class="pagination pagination-sm no-margin">
                      <li><a style="font-weight: bold;font-size: 25px;" onclick='move_week_backward();return false;'>&laquo;</a></li>
                      <li><a style="font-weight: bold;font-size: 25px;" id="id_current_week">2/2 ~ 2/5</a></li>
                      <li><a style="font-weight: bold;font-size: 25px;" onclick='move_week_forward();return false;'>&raquo;</a></li>
                    </ul>

                </th>
              </tr>
            </thead>
            <tbody style="text-align:center;">
              <tr>
                <!-- <td style="background-color:#8d8f8e;border-color:white;width:50px;"></td> -->
                <td style="background-color:#8d8f8e;border-color:white;color:white;width:180px;"><div style="font-weight: bold;font-size: 25px;">MON</div><div>星期一</div><div id='monday_date'>2/2</div></td>
                <td style="background-color:#8d8f8e;border-color:white;color:white;width:180px;"><div style="font-weight: bold;font-size: 25px;">TUE</div><div>星期二</div><div id='tuesday_date'>2/3</div></td>
                <td style="background-color:#8d8f8e;border-color:white;color:white;width:180px;"><div style="font-weight: bold;font-size: 25px;">WED</div><div>星期三</div><div id='wednesday_date'>2/4</div></td>
                <td style="background-color:#8d8f8e;border-color:white;color:white;width:180px;"><div style="font-weight: bold;font-size: 25px;">THU</div><div>星期四</div><div id='thursday_date'>2/5</div></td>
                <td style="background-color:#8d8f8e;border-color:white;color:white;width:180px;"><div style="font-weight: bold;font-size: 25px;">FRI</div><div>星期五</div><div id='friday_date'>2/6</div></td>
              </tr>

              <tr style="background-color:#eae6ed;">
                <td style="border-color:black;vertical-align:middle;" id="monday_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_class"></td>
              </tr>

                <!-- <tr style="background-color:#eae6ed;">
                  <td style="border-color:black;width:40px;" id="time_1_class">08:00 <br>|<br> 09:00</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_1_class"></td>
                </tr>
                <tr>
                  <td style="border-color:black;width:40px;" id="time_2_class">09:00 <br>|<br> 10:00</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_2_class"></td>
                </tr>
                <tr style="background-color:#eae6ed;">
                  <td style="border-color:black;width:40px;" id="time_3_class">10:00 <br>|<br> 11:00</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_3_class"></td>
                </tr>
                <tr>
                  <td style="border-color:black;width:40px;" id="time_4_class">11:00 <br>|<br> 12:00</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_4_class"></td>
                </tr>

              <tr style="background-color:#eae6ed;">
                <td style="border-color:black;width:50px;" id="time_5_class">12:00 <br>|<br> 13:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_5_class"></td>
              </tr>
              <tr>
                <td style="border-color:black;width:50px;" id="time_6_class">13:00 <br>|<br> 14:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_6_class"></td>
              </tr>
              <tr style="background-color:#eae6ed;">
                <td style="border-color:black;width:50px;" id="time_7_class">14:00 <br>|<br> 15:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_7_class"></td>
              </tr>
              <tr>
                <td style="border-color:black;width:50px;" id="time_8_class">15:00 <br>|<br> 16:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_8_class"></td>
              </tr> -->

            </tbody>
            <tfoot>
              <tr>
                <td style="border-color:black;" colspan="8"></td>
              </tr>
            </tfoot>
        </table>

        </div>
        <!-- /.box-body-->

      </div>
      <!-- /.box-->

    </div>
    <!-- /.col -->

  </div>
<!-- /.row -->

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
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/curriculumschedule.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/curriculumschedule_src/00-curriculumschedule.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
