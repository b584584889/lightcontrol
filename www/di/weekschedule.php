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

 <div class="row">

  <div class="col-md-12 col-sd-12">

      <!-- weekschedule -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $weekschedule_list; ?></h3>
          <div class="box-tools pull-right">
          </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body">

        <table class="table table-bordered" style="border-color:black;border:3px #111111 solid;">
            <thead>
              <tr style="background-color:#acdafb;border-color:black;">
                <th style="text-align:center;border-color:black;" colspan="8" id="schedule_title">年度日課表</th>
              </tr>
            </thead>
            <tbody style="text-align:center;">
              <tr>
                <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="4">班級</td>
                <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="4">導師</td>
              </tr>
              <tr>
                <td style="border-color:black;" colspan="4" id="class_title"></td>
                <td style="border-color:black;" colspan="4" id="class_teacher"></td>
              </tr>

              <tr>
                  <td style="border-color:black;" colspan="8"> </td>
              </tr>

              <tr>
                <td style="background-color:#c0e7cd;border-color:black;width:50px;">午別</td>
                <td style="background-color:#c0e7cd;border-color:black;width:100px;" colspan="2">星期\科目<br>節次\時間</td>
                <td style="background-color:#c0e7cd;border-color:black;">星期一</td>
                <td style="background-color:#c0e7cd;border-color:black;">星期二</td>
                <td style="background-color:#c0e7cd;border-color:black;">星期三</td>
                <td style="background-color:#c0e7cd;border-color:black;">星期四</td>
                <td style="background-color:#c0e7cd;border-color:black;">星期五</td>
              </tr>

              <tr>
                <td style="background-color:#e5e7c0;border-color:black;width:50px;vertical-align:middle;" rowspan="8">上午</td>
                <td style="border-color:black;width:120px;" colspan="2">07:10 ~ 07:30</td>
                <td style="border-color:black;" colspan="5">上學時間暨交通導護時間</td>
              </tr>

              <tr>
               <td style="border-color:black;width:120px;" colspan="2">07:20 ~ 07:45</td>
               <td style="border-color:black;" colspan="5">清掃時間</td>
              </tr>

                <tr>
                 <td style="border-color:black;width:120px;" colspan="2">07:45 ~ 08:15</td>
                 <td style="border-color:black;" colspan="5">導師時間 / 全校至中庭升旗暨晨間運動</td>
                </tr>

                <tr>
                 <td style="border-color:black;width:120px;" colspan="2">08:15 ~ 08:25</td>
                 <td style="border-color:black;" colspan="5">下課</td>
                </tr>

                <tr style="background-color:#eae6ed;">
                  <td style="background-color:#e7dbc0;border-color:black;width:40px;vertical-align:middle;">1</td>
                  <td style="border-color:black;width:40px;" id="time_1_class">08:25 <br>|<br> 09:10</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_1_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_1_class"></td>
                </tr>
                <tr>
                  <td style="background-color:#e7dbc0;border-color:black;width:40px;vertical-align:middle;">2</td>
                  <td style="border-color:black;width:40px;" id="time_2_class">09:20 <br>|<br> 10:05</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_2_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_2_class"></td>
                </tr>
                <tr style="background-color:#eae6ed;">
                  <td style="background-color:#e7dbc0;border-color:black;width:40px;vertical-align:middle;">3</td>
                  <td style="border-color:black;width:40px;" id="time_3_class">10:15 <br>|<br> 11:00</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_3_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_3_class"></td>
                </tr>
                <tr>
                  <td style="background-color:#e7dbc0;border-color:black;width:40px;vertical-align:middle;">4</td>
                  <td style="border-color:black;width:40px;" id="time_4_class">11:10 <br>|<br> 11:55</td>
                  <td style="border-color:black;vertical-align:middle;" id="monday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="tuesday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="wednesday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="thursday_4_class"></td>
                  <td style="border-color:black;vertical-align:middle;" id="friday_4_class"></td>
                </tr>

                <tr>
                 <td style="border-color:black;width:120px;" colspan="3">11:55 ~ 12:20</td>
                 <td style="border-color:black;" colspan="5">午餐時間</td>
                </tr>
                <tr>
                 <td style="border-color:black;width:120px;" colspan="3">12:20 ~ 12:25</td>
                 <td style="border-color:black;" colspan="5">垃圾清運及廚餘處理（每日）資源回收（二、五 12:15 ~ 12:25)</td>
                </tr>
                <tr>
                 <td style="border-color:black;width:120px;" colspan="3">12:25 ~ 13:15</td>
                 <td style="border-color:black;" colspan="5">午休時間</td>
                </tr>


              <tr style="background-color:#eae6ed;">
                <td style="background-color:#e5e7c0;border-color:black;width:50px;vertical-align:middle;" rowspan="4">下午</td>
                <td style="background-color:#e7dbc0;border-color:black;width:50px;vertical-align:middle;">5</td>
                <td style="border-color:black;width:50px;" id="time_5_class">13:20 <br>|<br> 14:05</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_5_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_5_class"></td>
              </tr>
              <tr>
                <td style="background-color:#e7dbc0;border-color:black;width:50px;vertical-align:middle;">6</td>
                <td style="border-color:black;width:50px;" id="time_6_class">14:15 <br>|<br> 15:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_6_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_6_class"></td>
              </tr>
              <tr style="background-color:#eae6ed;">
                <td style="background-color:#e7dbc0;border-color:black;width:50px;vertical-align:middle;">7</td>
                <td style="border-color:black;width:50px;" id="time_7_class">15:20 <br>|<br> 16:05</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_7_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_7_class"></td>
              </tr>
              <tr>
                <td style="background-color:#e7dbc0;border-color:black;width:50px;vertical-align:middle;">8</td>
                <td style="border-color:black;width:50px;" id="time_8_class">16:15 <br>|<br> 17:00</td>
                <td style="border-color:black;vertical-align:middle;" id="monday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="tuesday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="wednesday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="thursday_8_class"></td>
                <td style="border-color:black;vertical-align:middle;" id="friday_8_class"></td>
              </tr>

              <tr>
               <td style="border-color:black;width:120px;" colspan="3">17:00 ~ 17:15</td>
               <td style="border-color:black;" colspan="5">放學</td>
              </tr>


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
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/weekschedule.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/weekschedule_src/00-weekschedule.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
