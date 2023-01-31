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
    <title>SMIMS VILS IoT - Survey</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilssurvey_title;?></h3>

          <span>&nbsp;</span>
          <a class='btn btn-success btn-xs' onclick='create_new_survey();'><?php echo $vilssurvey_createnewsurvey;?></a>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_survey_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body">

          <table class="table table-bordered" id="SURVEY_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>事項名稱</th>
                <th>事項說明</th>
                <th>選擇類型</th>
                <th>事項列表</th>
                <th style="width: 300px">子集合權限</th>
                <th>編輯</th>
              </tr>
            </thead>
            <tbody id="SURVEY_TABLE_BODY">

            </tbody>
          </table>
          <!-- <div class="btn-group">
              <button class="btn btn-block btn-warning btn-lg" id="createnewsurveybutton" ><i class="fa fa-plus"></i><?php echo $vilssurvey_createnewsurvey;?></button>
          </div> -->

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
        var gl_maingroups = "<?php echo $_SESSION["maingroups"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/survey.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/survey_src/00-survey.js"></script> -->
</html>
