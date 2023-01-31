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
    <title>SMIMS VILS IoT - Curriculum</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilscurriculum_title;?></h3>

          <?php if ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 5 || $_SESSION["groupid"] == 6)
          {
              echo '<span>&nbsp;</span>';
              echo '<a class="btn btn-success btn-xs" onclick="create_new_curriculum();">'; echo $vilscurriculum_createnew; echo '</a>';

              echo '<span>&nbsp;</span>';
              echo '<div class="btn-group" id="curriculumprefix_area">';
              echo '    <span class="badge bg-aqua-active" id="curriculumprefix_label">課程代碼:</span>';
              echo '</div>';

              if ($_SESSION["groupid"] == 6)
              {
                  echo '<div class="btn-group">';
                  echo '      <button type="button" title="編輯課程代碼" onclick="edit_curriculumprefix();return false;" id="curriculumprefix_btn"><i class="fa fa-edit" id="curriculumprefix_img"></i></button>';
                  echo '</div>';
              }
          }
          ?>

          <div class="box-tools pull-right">
              <!-- <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul> -->
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_curriculum_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body">
          <div class="nav-tabs-custom">
            <ul class="nav nav-tabs">
              <li class='active'><a href="#loc_tab_curriculum" data-toggle='tab'><span class='badge bg-yellow'><?php echo $vilscurriculumcode_edit;?></span></a></li>
              <li><a href="#loc_tab_shared_curriculum" data-toggle='tab'><span class='badge bg-blue'><?php echo $vilscurriculumcode_shared;?></span></a></li>

            </ul>
            <div class="tab-content">

              <div class='tab-pane active table-responsive' id='loc_tab_curriculum'>
                <div class="box">
                  <div class="box-header with-border">
                    <h3 class="box-title"></h3>
                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                        <li><a href="#" id="id_current_page_num">1</a></li>
                        <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                      </ul>
                    </div>
                  </div>
                  <!-- /.box-header -->
                  <div class="box-body no-padding">
                    <table class="table table-bordered" id="CURRICULUM_TABLE">
                      <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                          <th style="width: 30px">#</th>
                          <th style="width: 100px">發起人</th>
                          <th style="width: 200px">發起單位(主集合)</th>
                          <th style="width: 150px">類別</th>
                          <th style="width: 100px">是否共享</th>
                          <th>分享對象(主集合)</th>
                          <th>開放對象(子集合)</th>
                          <th style="width: 150px">課程號碼</th>
                          <th style="width: 50px">編輯</th>
                        </tr>
                      </thead>
                      <tbody id="CURRICULUM_TABLE_BODY">

                      </tbody>
                    </table>
                  </div>
                  <!-- /.box-body -->
                </div>
                <!-- /.box -->
              </div>
              <!-- /.tab-pane -->

              <div class='tab-pane       table-responsive' id='loc_tab_shared_curriculum'>
                <div class="box">
                  <div class="box-header with-border">
                    <h3 class="box-title"></h3>
                      <div class="box-tools pull-right">
                        <ul class="pagination pagination-sm no-margin">
                          <li><a href="#" onclick='move_shared_page_backward();return false;'>&laquo;</a></li>
                          <li><a href="#" id="id_current_shared_page_num">1</a></li>
                          <li><a href="#" onclick='move_shared_page_forward();return false;'>&raquo;</a></li>
                        </ul>
                      </div>
                    </div>
                    <!-- /.box-header -->
                    <div class="box-body no-padding">
                      <table class="table table-bordered" id="SHARED_CURRICULUM_TABLE">
                        <thead>
                          <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 30px">#</th>
                            <th style="width: 100px">發起人</th>
                            <th style="width: 200px">發起單位(主集合)</th>
                            <th style="width: 150px">類別</th>
                            <th style="width: 100px">課程名稱</th>
                            <th>開放對象(子集合)</th>
                            <th style="width: 150px">課程號碼</th>
                            <th style="width: 50px">編輯</th>
                          </tr>
                        </thead>
                        <tbody id="SHARED_CURRICULUM_TABLE_BODY">

                        </tbody>
                      </table>
                  </div>
                  <!-- /.box-body -->
                </div>
                <!-- /.box -->
              </div>
              <!-- /.tab-pane -->

            </div>
            <!-- /.tab-content -->
          </div>
          <!-- nav-tabs-custom -->
        </div>
        <!-- /.box-body -->

        <div class="box-footer" style="display:<?php if ($_SESSION['groupid'] == 1 || $_SESSION['groupid'] == 2) echo 'block'; else echo 'none';?>">

            <div class="box">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vilscurriculumtype_title;?></h3>

                <span>&nbsp;</span>
                <a class="btn btn-success btn-xs" onclick="create_new_curriculumtype();"><?php echo $vilscurriculumtype_createnew;?> </a>

             </div>
             <div class="box-body">

                <table class="table table-bordered">
                  <thead>
                    <tr style="background-color:rgb(216, 210, 215);">
                      <th style="width: 30px">#</th>
                      <th>課程類別</th>
                      <th>編輯</th>
                    </tr>
                  </thead>
                  <tbody id="CURRICULUM_TYPE_TABLE_BODY">
                  </tbody>
                </table>

             </div>
        </div>
        <!-- /.box-footer -->

      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/curriculum.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/curriculum_src/00-curriculum.js?date=<?php echo $UPDATE_DATE;?>"></script> -->
</html>
