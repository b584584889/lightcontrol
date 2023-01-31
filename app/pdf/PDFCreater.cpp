/*
 *  PDFCreater.cpp
 *
 *  Created on: 2019/08/21
 *      Author: ikki
 */
#include "PDFCreater.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

#include <unistd.h>

#include "../common.h"

jmp_buf env;

void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void * user_data)
{
    vilslog::printf("====================\n");
    vilslog::printf("PDFCreater::error_handler() ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    vilslog::printf("====================\n");
    longjmp(env, 1);
}

PDFCreater::PDFCreater()
{
    //create_ihome_report("/Library/WebServer/Documents/upload/ihome_report.pdf");
    //create_ihome_report("../ihome_report/ihome_report.pdf");
    //test_pdf_creater();
}

PDFCreater::~PDFCreater()
{

}

void PDFCreater::create_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, UserStruct * user, std::list<EmployeeEventReportStruct*> * eventlist)
{
    HPDF_Doc pdf;
    HPDF_Page page;
    //HPDF_Font font_en_num;
    HPDF_Font font_big5_bold;
    HPDF_Font font_big5_light;
    HPDF_Font font_big5_regular;
    HPDF_Destination dst;

    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::create_event_report() start.\n");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf)
    {
        vilslog::printf ("PDFCreater::create_event_report() error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (pdf);
        vilslog::printf ("PDFCreater::create_event_report() error: setjmp(env)\n");
        return;
    }

    // traditional Chinese Encodings
    HPDF_UseCNTEncodings(pdf);
    HPDF_UseCNTFonts(pdf);

    std::string appfilepath = APP_DIRECTORY;
    std::string font_big5_bold_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Bold.ttf";
    std::string font_big5_light_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Light.ttf";
    std::string font_big5_regular_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Regular.ttf";

    std::string font_big5_bold_name = HPDF_LoadTTFontFromFile(pdf, font_big5_bold_filepath.c_str(), HPDF_TRUE);
    font_big5_bold = HPDF_GetFont (pdf, font_big5_bold_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_light_name = HPDF_LoadTTFontFromFile(pdf, font_big5_light_filepath.c_str(), HPDF_TRUE);
    font_big5_light = HPDF_GetFont (pdf, font_big5_light_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_regular_name = HPDF_LoadTTFontFromFile(pdf, font_big5_regular_filepath.c_str(), HPDF_TRUE);
    font_big5_regular = HPDF_GetFont (pdf, font_big5_regular_name.c_str(), "ETen-B5-H");// ETen-B5-V

    // create default-font
    //font_en_num = HPDF_GetFont (pdf, "Helvetica", NULL);

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    // add a new page object.
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (page, 842);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    //std::list<EmployeeEventReportStruct*>::iterator event = eventlist->begin();

    std::string report_emp = "";
    std::string report_emp_orgName = "";
    std::string report_emp_jobTitle = "";
    if (user != NULL)
    {
        report_emp = user->name;
        report_emp_orgName = user->unit;
        report_emp_jobTitle = user->title;
    }

    vilslog::printf("PDFCreater::create_event_report() [%s] [%s][%s][%s] [%s]->[%s] eventlist->size[%d]\n",
        report_date.c_str(), report_emp.c_str(), report_emp_orgName.c_str(), report_emp_jobTitle.c_str(), report_start_date.c_str(), report_end_date.c_str(), (int)eventlist->size());

    //
    // 左上 ihome 圖片
    //
    if (user->empid.length() > 0)
    {
        std::string appfilepath = APP_DIRECTORY;
        std::string image_filepath = appfilepath + "/pdf/images/N32.jpg";
        draw_image(pdf, PDF_IMAGE_JPG, image_filepath.c_str(), 50, HPDF_Page_GetHeight (page) - 150, 100);
    }
    else
    {
        std::string appfilepath = APP_DIRECTORY;
        std::string image1_filepath = appfilepath + "/pdf/images/VILS_logo-2x.png";
        std::string image2_filepath = appfilepath + "/pdf/images/smims_logo.jpg";
        draw_image(pdf, PDF_IMAGE_PNG, image1_filepath.c_str(), 75, HPDF_Page_GetHeight (page) - 100, 50);
        draw_image(pdf, PDF_IMAGE_JPG, image2_filepath.c_str(), 50, HPDF_Page_GetHeight (page) - 150, 100);
    }

    //
    // 右上 報告名稱
    //
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "人員求助處理報告", font_big5_bold, 20);

    // 右上 報告產生日期
    std::string report_create_date = "報告產生日期: ";
    report_create_date.append(report_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 125, report_create_date, font_big5_light, 10);

    // 右上 報告資料區間
    std::string report_date_range = "報告資料區間: ";
    report_date_range.append(report_start_date);
    report_date_range.append(" 至 ");
    report_date_range.append(report_end_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 140, report_date_range, font_big5_light, 10);

    // 處理人員
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 220, "處理人員", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 225, 200, HPDF_Page_GetHeight (page) - 225, 0.5);

    // 處理人員 名稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 240, report_emp, font_big5_bold, 14);

    // 處理人員 部門
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 260, report_emp_orgName, font_big5_regular, 12);

    // 處理人員 職稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 280, report_emp_jobTitle, font_big5_regular, 12);

    // 事件列表
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 320, "事件列表", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 325, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 325, 0.5);

    // 日期時間
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 340, "日期時間", font_big5_light, 10);

    // 求助住戶
    draw_big5_text(page, 150, HPDF_Page_GetHeight (page) - 340, "求助住戶", font_big5_light, 10);

    // 開始處理時間
    draw_big5_text(page, 260, HPDF_Page_GetHeight (page) - 340, "開始處理時間", font_big5_light, 10);

    // 事件說明
    draw_big5_text(page, 350, HPDF_Page_GetHeight (page) - 340, "事件說明", font_big5_light, 10);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 345, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 345, 0.5);


    //=======================================
    // 事件列表
    //
    int top_y = HPDF_Page_GetHeight (page) - 360;
    int draw_height = 0;
    for (std::list<EmployeeEventReportStruct*>::iterator event = eventlist->begin(); event != eventlist->end(); event++)
    {
        std::string event_start_datetime = (*event)->eventtime;
        std::string resident_name = (*event)->resident_name;
        resident_name.append(" ");
        resident_name.append((*event)->resident_roomNo);
        std::string reponsetime = (*event)->reponsetime;
        std::string comment = (*event)->comment;

        draw_height = draw_events(page, 50, top_y, event_start_datetime, resident_name, reponsetime, comment, font_big5_light, 10);
        if (draw_height == 0)
            draw_height = 10;

        //vilslog::printf("PDFCreater::create_ihome_report() draw_height[%d]\n", draw_height);

        top_y -= (draw_height + 10);
        if(top_y >= HPDF_Page_GetHeight (page))
        {
            // add a new page object.
            page = HPDF_AddPage (pdf);
            top_y = HPDF_Page_GetHeight (page) - 50;
        }
    }

    if ( (int)eventlist->size() == 0 )
    {
        draw_events(page, 50, top_y, "無資料", "", "", "", font_big5_light, 10);
    }

    //
    // int top_y = HPDF_Page_GetHeight (page) - 360;
    // int draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "1風雨過大, 導致窗戶滲水!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "2風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "3風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "4風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // if(top_y >= HPDF_Page_GetHeight (page))
    // {
    //     // add a new page object.
    //     page = HPDF_AddPage (pdf);
    //     top_y = HPDF_Page_GetHeight (page) - 50;
    // }
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "5風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // if(top_y >= HPDF_Page_GetHeight (page))
    // {
    //     // add a new page object.
    //     page = HPDF_AddPage (pdf);
    //     top_y = HPDF_Page_GetHeight (page) - 50;
    // }
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "6風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // //vilslog::printf("top_y[%d]\n",top_y);
    // if(top_y <= 0)
    // {
    //     // add a new page object.
    //     page = HPDF_AddPage (pdf);
    //     top_y = HPDF_Page_GetHeight (page) - 50;
    // }
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "7風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // //vilslog::printf("top_y[%d]\n",top_y);
    // if(top_y <= 0)
    // {
    //     // add a new page object.
    //     page = HPDF_AddPage (pdf);
    //     top_y = HPDF_Page_GetHeight (page) - 50;
    // }
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "8風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);
    //
    // top_y -= (draw_height + 10);
    // //vilslog::printf("top_y[%d]\n",top_y);
    // if(top_y <= 0)
    // {
    //     // add a new page object.
    //     page = HPDF_AddPage (pdf);
    //     top_y = HPDF_Page_GetHeight (page) - 50;
    // }
    // draw_height = draw_events(page, 50, top_y, "2019/08/23 10:32:56", "顧裔芳 0505(A06-12)", "10:33:06", "9風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 10);

    // // 日期時間
    // draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 360, "2019/08/23 10:32:56", font_big5_light, 10);
    //
    // // 求助住戶
    // draw_big5_text(page, 150, HPDF_Page_GetHeight (page) - 360, "顧裔芳 0505(A06-12)", font_big5_bold, 10);
    //
    // // 開始處理時間
    // draw_big5_text(page, 250, HPDF_Page_GetHeight (page) - 360, "10:33:06", font_big5_light, 10);
    //
    // // 事件說明
    // //draw_big5_text(page, 350, HPDF_Page_GetHeight (page) - 360, "因爲風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 12);
    // //draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360,  HPDF_Page_GetWidth(page) - 350 - 50, "因爲風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 12);
    // int draw_height = 0;
    // int font_size = 10;
    // draw_height += draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360 - draw_height ,  HPDF_Page_GetWidth(page) - 350 - 50, "123風雨過大, 導致窗戶滲水!", font_big5_light, font_size);
    //
    // draw_height += draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360 - draw_height,  HPDF_Page_GetWidth(page) - 350 - 50, "123風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, font_size);
    // draw_height += draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360 - draw_height,  HPDF_Page_GetWidth(page) - 350 - 50, "123風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水", font_big5_light, font_size);
    // draw_height += draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360 - draw_height,  HPDF_Page_GetWidth(page) - 350 - 50, "123風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, font_size);

    // draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 360,  HPDF_Page_GetWidth(page) - 350 - 50, "風雨過大", font_big5_light, 20);
    // draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 400,  HPDF_Page_GetWidth(page) - 350 - 50, "風雨過大, 導致窗戶滲水", font_big5_light, 20);
    // draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 460,  HPDF_Page_GetWidth(page) - 350 - 50, "風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!", font_big5_light, 20);
    // draw_big5_rect_text(page, 350, HPDF_Page_GetHeight (page) - 560,  HPDF_Page_GetWidth(page) - 350 - 50, "風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!, 風雨過大, 導致窗戶滲水,", font_big5_light, 20);
    //=======================================


    // save the document to a file
    HPDF_SaveToFile (pdf, pdf_filename.c_str());

    // clean up
    HPDF_Free (pdf);

    vilslog::printf("PDFCreater::create_event_report() end.\n");
    vilslog::printf("\n==============================\n");
}

void PDFCreater::create_inout_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<AreaInOutEventStruct*> * nodeAreaInOutList)
{
    std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = pair_inout_event(nodeAreaInOutList);

    vilslog::printf ("PDFCreater::create_inout_event_report() nodeid[%s] pair_inout_events->size()[%d]\n",
        node->nodeid, (int)pair_inout_events->size());

    create_inout_event_report(pdf_filename, report_date, report_start_date, report_end_date, node, pair_inout_events);

    clear_Pair_AreaInOutEventStruct_list(pair_inout_events);
    delete pair_inout_events;
}

void PDFCreater::create_inout_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<Pair_AreaInOutEventStruct*> * pair_inout_events)
{
    HPDF_Doc pdf;
    HPDF_Page page;
    //HPDF_Font font_en_num;
    HPDF_Font font_big5_bold;
    HPDF_Font font_big5_light;
    HPDF_Font font_big5_regular;
    HPDF_Destination dst;

    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::create_inout_event_report() start.\n");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf)
    {
        vilslog::printf ("PDFCreater::create_inout_event_report() error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (pdf);
        vilslog::printf ("PDFCreater::create_inout_event_report() error: setjmp(env)\n");
        return;
    }

    // traditional Chinese Encodings
    HPDF_UseCNTEncodings(pdf);
    HPDF_UseCNTFonts(pdf);

    std::string appfilepath = APP_DIRECTORY;
    std::string font_big5_bold_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Bold.ttf";
    std::string font_big5_light_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Light.ttf";
    std::string font_big5_regular_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Regular.ttf";

    std::string font_big5_bold_name = HPDF_LoadTTFontFromFile(pdf, font_big5_bold_filepath.c_str(), HPDF_TRUE);
    font_big5_bold = HPDF_GetFont (pdf, font_big5_bold_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_light_name = HPDF_LoadTTFontFromFile(pdf, font_big5_light_filepath.c_str(), HPDF_TRUE);
    font_big5_light = HPDF_GetFont (pdf, font_big5_light_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_regular_name = HPDF_LoadTTFontFromFile(pdf, font_big5_regular_filepath.c_str(), HPDF_TRUE);
    font_big5_regular = HPDF_GetFont (pdf, font_big5_regular_name.c_str(), "ETen-B5-H");// ETen-B5-V

    // create default-font
    //font_en_num = HPDF_GetFont (pdf, "Helvetica", NULL);

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    // add a new page object.
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (page, 842);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    std::string image1_filepath = appfilepath + "/pdf/images/VILS_logo-2x.png";
    std::string image2_filepath = appfilepath + "/pdf/images/smims_logo.jpg";
    draw_image(pdf, PDF_IMAGE_PNG, image1_filepath.c_str(), 75, HPDF_Page_GetHeight (page) - 100, 50);
    draw_image(pdf, PDF_IMAGE_JPG, image2_filepath.c_str(), 50, HPDF_Page_GetHeight (page) - 150, 100);

    std::string report_user = "";
    std::string report_user_orgName = "";
    std::string report_user_jobTitle = "";

    if (node != NULL)
    {
        report_user = node->nodename;
        if ( node->nodetype == NODE_TYPE_TAG )
            report_user_jobTitle = getUserType(node->tafid);
        else
        if ( node->nodetype == NODE_TYPE_LOCATOR )
            report_user_jobTitle = getLocatorType(node->tafid);
    }

    //
    // 右上 報告名稱
    //
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "人員到訪區域報告", font_big5_bold, 20);

    // 右上 報告產生日期
    std::string report_create_date = "報告產生日期: ";
    report_create_date.append(report_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 125, report_create_date, font_big5_light, 10);

    // 右上 報告資料區間
    std::string report_date_range = "報告資料區間: ";
    report_date_range.append(report_start_date);
    report_date_range.append(" 至 ");
    report_date_range.append(report_end_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 140, report_date_range, font_big5_light, 10);

    // 處理人員
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 220, "人員", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 225, 200, HPDF_Page_GetHeight (page) - 225, 0.5);

    // 處理人員 名稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 240, report_user, font_big5_bold, 14);

    // 處理人員 部門
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 260, report_user_orgName, font_big5_regular, 12);

    // 處理人員 職稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 280, report_user_jobTitle, font_big5_regular, 12);

    // 事件列表
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 320, "到訪記錄", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 325, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 325, 0.5);

    // 進入 日期時間
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 340, "進入時間", font_big5_light, 10);

    // 離開 日期時間
    draw_big5_text(page, 150, HPDF_Page_GetHeight (page) - 340, "離開時間", font_big5_light, 10);

    // 到訪區域
    draw_big5_text(page, 260, HPDF_Page_GetHeight (page) - 340, "到訪區域", font_big5_light, 10);

    // 離開時間
    //draw_big5_text(page, 350, HPDF_Page_GetHeight (page) - 340, "離開時間", font_big5_light, 10);

    // 停留時間
    draw_big5_text(page, 440, HPDF_Page_GetHeight (page) - 340, "停留時間", font_big5_light, 10);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 345, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 345, 0.5);

    //=======================================
    // 事件列表
    //
    int top_y = HPDF_Page_GetHeight (page) - 360;
    int draw_height = 0;
    for (std::list<Pair_AreaInOutEventStruct*>::iterator event = pair_inout_events->begin(); event != pair_inout_events->end(); event++)
    {
        std::string area_in_datetime = "";
        std::string area_out_datetime = "";
        std::string total_time = "0";

        if ( (*event)->type.compare("single") == 0 )
        {
            if ( (*event)->event1->action == 1 )
                area_in_datetime = (*event)->event1->DataTime;
            else
                area_out_datetime = (*event)->event1->DataTime;
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            area_in_datetime = (*event)->event1->DataTime;
            area_out_datetime = (*event)->event2->DataTime;

            time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
            time_t t_out = StringDateTimeTotime_t((*event)->event2->DataTime);
            time_t t_diff = t_out - t_in;
            total_time = time_tToTimerString(t_diff);
        }
        std::string area_name = (*event)->event1->AreaName;

        vilslog::printf("PDFCreater::create_inout_event_report() area_in_datetime[%s] area_out_datetime[%s] area_name[%s] total_time[%s]\n",
            area_in_datetime.c_str(), area_out_datetime.c_str(), area_name.c_str(), total_time.c_str());

        draw_height = draw_inout_events(page, 50, top_y, area_in_datetime, area_out_datetime, area_name, total_time, font_big5_light, 10);

        // Draw line
        draw_line(page, 50, top_y - 2, HPDF_Page_GetWidth(page) - 50, top_y - 2, 0.5);

        if (draw_height == 0)
            draw_height = 10;
        top_y -= (draw_height + 10);

        // vilslog::printf("PDFCreater::create_inout_event_report() top_y[%d] HPDF_Page_GetHeight(page)[%d]\n", top_y, (int)HPDF_Page_GetHeight(page));

        //if(top_y >= HPDF_Page_GetHeight (page))
        if(top_y <= 20)
        {
            // add a new page object.
            page = HPDF_AddPage (pdf);
            top_y = HPDF_Page_GetHeight (page) - 50;
        }
    }

    if ( (int)pair_inout_events->size() == 0 )
    {
        draw_events(page, 50, top_y, "無資料", "", "", "", font_big5_light, 10);
    }

    //=======================================


    // save the document to a file
    HPDF_SaveToFile (pdf, pdf_filename.c_str());

    // clean up
    HPDF_Free (pdf);

    vilslog::printf("PDFCreater::create_inout_event_report() end.\n");
    vilslog::printf("\n==============================\n");
}

void PDFCreater::create_firedevent_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<FiredEventStruct*> * nodeFiredEventsList)
{
    HPDF_Doc pdf;
    HPDF_Page page;
    //HPDF_Font font_en_num;
    HPDF_Font font_big5_bold;
    HPDF_Font font_big5_light;
    HPDF_Font font_big5_regular;
    HPDF_Destination dst;

    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::create_firedevent_report() start.\n");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf)
    {
        vilslog::printf ("PDFCreater::create_firedevent_report() error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (pdf);
        vilslog::printf ("PDFCreater::create_firedevent_report() error: setjmp(env)\n");
        return;
    }

    // traditional Chinese Encodings
    HPDF_UseCNTEncodings(pdf);
    HPDF_UseCNTFonts(pdf);

    std::string appfilepath = APP_DIRECTORY;
    std::string font_big5_bold_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Bold.ttf";
    std::string font_big5_light_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Light.ttf";
    std::string font_big5_regular_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Regular.ttf";

    std::string font_big5_bold_name = HPDF_LoadTTFontFromFile(pdf, font_big5_bold_filepath.c_str(), HPDF_TRUE);
    font_big5_bold = HPDF_GetFont (pdf, font_big5_bold_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_light_name = HPDF_LoadTTFontFromFile(pdf, font_big5_light_filepath.c_str(), HPDF_TRUE);
    font_big5_light = HPDF_GetFont (pdf, font_big5_light_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_regular_name = HPDF_LoadTTFontFromFile(pdf, font_big5_regular_filepath.c_str(), HPDF_TRUE);
    font_big5_regular = HPDF_GetFont (pdf, font_big5_regular_name.c_str(), "ETen-B5-H");// ETen-B5-V

    // create default-font
    //font_en_num = HPDF_GetFont (pdf, "Helvetica", NULL);

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    // add a new page object.
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (page, 842);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    std::string image1_filepath = appfilepath + "/pdf/images/VILS_logo-2x.png";
    std::string image2_filepath = appfilepath + "/pdf/images/smims_logo.jpg";
    draw_image(pdf, PDF_IMAGE_PNG, image1_filepath.c_str(), 75, HPDF_Page_GetHeight (page) - 100, 50);
    draw_image(pdf, PDF_IMAGE_JPG, image2_filepath.c_str(), 50, HPDF_Page_GetHeight (page) - 150, 100);

    std::string report_user = "";
    std::string report_user_orgName = "";
    std::string report_user_jobTitle = "";

    if (node != NULL)
    {
        report_user = node->nodename;
        if ( node->nodetype == NODE_TYPE_TAG )
            report_user_jobTitle = getUserType(node->tafid);
        else
        if ( node->nodetype == NODE_TYPE_LOCATOR )
            report_user_jobTitle = getLocatorType(node->tafid);
    }


    //
    // 右上 報告名稱
    //
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "事件報告", font_big5_bold, 20);

    // 右上 報告產生日期
    std::string report_create_date = "報告產生日期: ";
    report_create_date.append(report_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 125, report_create_date, font_big5_light, 10);

    // 右上 報告資料區間
    std::string report_date_range = "報告資料區間: ";
    report_date_range.append(report_start_date);
    report_date_range.append(" 至 ");
    report_date_range.append(report_end_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 140, report_date_range, font_big5_light, 10);

    // 處理人員
    if ( node->nodetype == NODE_TYPE_TAG )
        draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 220, "人員", font_big5_light, 12);
    else
        draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 220, "裝置", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 225, 200, HPDF_Page_GetHeight (page) - 225, 0.5);

    // 處理人員 名稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 240, report_user, font_big5_bold, 14);

    // 處理人員 部門
    //draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 260, report_user_orgName, font_big5_regular, 12);

    // 處理人員 職稱
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 260, report_user_jobTitle, font_big5_regular, 12);

    // 事件列表
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 320, "事件記錄", font_big5_light, 12);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 325, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 325, 0.5);

    // 發生日期時間
    draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 340, "發生時間", font_big5_light, 10);

    // 人員
    draw_big5_text(page, 150, HPDF_Page_GetHeight (page) - 340, "人員", font_big5_light, 10);

    // 事件
    draw_big5_text(page, 225, HPDF_Page_GetHeight (page) - 340, "事件", font_big5_light, 10);

    // 到訪區域
    draw_big5_text(page, 275, HPDF_Page_GetHeight (page) - 340, "到訪區域", font_big5_light, 10);

    // 進入時間
    // draw_big5_text(page, 350, HPDF_Page_GetHeight (page) - 340, "進入時間", font_big5_light, 10);

    // 離開時間
    draw_big5_text(page, 350, HPDF_Page_GetHeight (page) - 340, "離開時間", font_big5_light, 10);

    // 停留時間
    draw_big5_text(page, 450, HPDF_Page_GetHeight (page) - 340, "停留時間", font_big5_light, 10);

    // Draw line
    draw_line(page, 50, HPDF_Page_GetHeight (page) - 345, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 345, 0.5);

    //=======================================
    // 事件列表
    //
    int top_y = HPDF_Page_GetHeight (page) - 360;
    int draw_height = 0;
    for (std::list<FiredEventStruct*>::iterator event = nodeFiredEventsList->begin(); event != nodeFiredEventsList->end(); event++)
    {
        Json::Value JSONresult;

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);
        delete reader;

        if (!parsingSuccessful)
        {
            continue;
        }

        std::string event_datetime = (*event)->datetime;
        std::string action = "";
        std::string user = "";
        std::string area = "";
        std::string area_in_datetime = "0";
        std::string area_out_datetime = "0";
        std::string total_time = "0";

        if ( (*event)->eventid == 9999 )
        {
            // "Alert" "Cancel" "Enter" "Leave"
            // {"projectid":"1","eventid":"9999","nodeid":"Locator0006","action":"Alert","areaid":"","issuetime":"0","locatorid":"Locator0006"}
            // {"projectid":"1","eventid":"9999","nodeid":"Locator0006","action":"Cancel","areaid":"","issuetime":"0","locatorid":"Locator0006"}
            // {"projectid":"1","eventid":"9999","nodeid":"Tag9902","action":"Leave","areaid":"area5265","issuetime":"0","locatorid":""}
            // {"projectid":"1","eventid":"9999","nodeid":"Tag9902","action":"Enter","areaid":"area9039","issuetime":"0","locatorid":""}
            action = JSONresult["action"].asString();
            area = JSONresult["areaid"].asString();
        }
        else
        if ( (*event)->eventid == 999 )
        {
            // "checkpoint" "checkpointpath"
            // {"type":"checkpointgroup","projectid":"1","GroupID":"85","GroupName":"巡檢路線6","action":"exceed","issuetime":"2019-06-14 16:30:02"}
            // {"type":"checkpoint","projectid":"1","GroupID":"85","PathIndex":"1","TagNodeId":"Tag0126","action":"longtime","NodeId":"Locator0177","issuetime":"2019-06-06 16:15:33"}
            // {"type":"checkpointpath","projectid":"1","GroupID":"15","PathIndex":"1","TagNodeId":"","action":"exceed","StartNodeId":"Locator0013","EndNodeId":"Locator1001","issuetime":"2019-04-29 10:01:25"}
            continue;
        }
        else
        if ( (*event)->eventid == 8888 )
        {
            // CallOut
            // {"projectid":"1","eventid":"8888","nodeid":"Locator0001","action":"CallOut","areaid":"","issuetime":"0","locatorid":"Locator0001"}
            action = JSONresult["action"].asString();
            area = JSONresult["areaid"].asString();
        }
        else
        if ( (*event)->eventid == 7777 )
        {
            // DOCTOR_EVENT
            // {"projectid":"1","eventid":"7777","nodeid":"Tag9902","action":"Enter","areaid":"area2829","issuetime":"0","locatorid":"Locator0177"}
            //  {"projectid":"1","eventid":"7777","nodeid":"Tag9902","action":"Leave","areaid":"area5376","issuetime":"0","locatorid":"Locator0006"}
            action = JSONresult["action"].asString();
            user = JSONresult["nodeid"].asString();
            area = JSONresult["areaid"].asString();
        }

        /*
        if ( (*event)->type.compare("single") == 0 )
        {
            if ( (*event)->event1->action == 1 )
                area_in_datetime = (*event)->event1->DataTime;
            else
                area_out_datetime = (*event)->event1->DataTime;
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            area_in_datetime = (*event)->event1->DataTime;
            area_out_datetime = (*event)->event2->DataTime;

            time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
            time_t t_out = StringDateTimeTotime_t((*event)->event2->DataTime);
            time_t t_diff = t_out - t_in;
            total_time = time_tToTimerString(t_diff);
        }
        std::string area_name = (*event)->event1->AreaName;
        */

        vilslog::printf("PDFCreater::create_firedevent_report() event_datetime[%s] user[%s] action[%s] area_name[%s] area_in_datetime[%s] area_out_datetime[%s] total_time[%s]\n",
            event_datetime.c_str(), user.c_str(), action.c_str(), area.c_str(), area_in_datetime.c_str(), area_out_datetime.c_str(), total_time.c_str());

        draw_height = draw_firedevent_events(page, 50, top_y, event_datetime, user, action, area, area_in_datetime, area_out_datetime, total_time, font_big5_light, 10);

        // Draw line
        draw_line(page, 50, top_y - 2, HPDF_Page_GetWidth(page) - 50, top_y - 2, 0.5);

        if (draw_height == 0)
            draw_height = 10;
        top_y -= (draw_height + 10);

        vilslog::printf("PDFCreater::create_firedevent_report() top_y[%d] HPDF_Page_GetHeight(page)[%d]\n", top_y, (int)HPDF_Page_GetHeight(page));

        //if(top_y >= HPDF_Page_GetHeight (page))
        if(top_y <= 20)
        {
            // add a new page object.
            page = HPDF_AddPage (pdf);
            top_y = HPDF_Page_GetHeight (page) - 50;
        }
    }

    if ( (int)nodeFiredEventsList->size() == 0 )
    {
        draw_events(page, 50, top_y, "無資料", "", "", "", font_big5_light, 10);
    }

    //=======================================


    // save the document to a file
    HPDF_SaveToFile (pdf, pdf_filename.c_str());

    // clean up
    HPDF_Free (pdf);

    vilslog::printf("PDFCreater::create_firedevent_report() end.\n");
    vilslog::printf("==============================\n");
}

void PDFCreater::create_inspection_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, PDFTableDataStruct * pdfData)
{
    HPDF_Doc pdf;
    HPDF_Page page;
    //HPDF_Font font_en_num;
    HPDF_Font font_big5_bold;
    HPDF_Font font_big5_light;
    //HPDF_Font font_big5_regular;
    HPDF_Destination dst;

    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::create_inspection_report() start.\n");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf)
    {
        vilslog::printf ("PDFCreater::create_inspection_report() error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (pdf);
        vilslog::printf ("PDFCreater::create_inspection_report() error: setjmp(env)\n");
        return;
    }

    // traditional Chinese Encodings
    HPDF_UseCNTEncodings(pdf);
    HPDF_UseCNTFonts(pdf);

    std::string appfilepath = APP_DIRECTORY;
    std::string font_big5_bold_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Bold.ttf";
    std::string font_big5_light_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Light.ttf";
    std::string font_big5_regular_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Regular.ttf";

    std::string font_big5_bold_name = HPDF_LoadTTFontFromFile(pdf, font_big5_bold_filepath.c_str(), HPDF_TRUE);
    font_big5_bold = HPDF_GetFont (pdf, font_big5_bold_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_light_name = HPDF_LoadTTFontFromFile(pdf, font_big5_light_filepath.c_str(), HPDF_TRUE);
    font_big5_light = HPDF_GetFont (pdf, font_big5_light_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_regular_name = HPDF_LoadTTFontFromFile(pdf, font_big5_regular_filepath.c_str(), HPDF_TRUE);
    //font_big5_regular = HPDF_GetFont (pdf, font_big5_regular_name.c_str(), "ETen-B5-H");// ETen-B5-V

    // create default-font
    //font_en_num = HPDF_GetFont (pdf, "Helvetica", NULL);

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    // add a new page object.
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (page, 842);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    std::string image1_filepath = appfilepath + "/pdf/images/VILS_logo-2x.png";
    std::string image2_filepath = appfilepath + "/pdf/images/smims_logo.jpg";
    draw_image(pdf, PDF_IMAGE_PNG, image1_filepath.c_str(), 75, HPDF_Page_GetHeight (page) - 100, 50);
    draw_image(pdf, PDF_IMAGE_JPG, image2_filepath.c_str(), 50, HPDF_Page_GetHeight (page) - 150, 100);

    //
    // 右上 報告名稱
    //
    if (pdfData->type.compare("daily") == 0 )
        draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "巡檢報告-日報表", font_big5_bold, 20);
    else
    if (pdfData->type.compare("weekly") == 0 )
        draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "巡檢報告-週報表", font_big5_bold, 20);
    else
    if (pdfData->type.compare("monthly") == 0 )
        draw_big5_text(page, HPDF_Page_GetWidth(page) - 250, HPDF_Page_GetHeight (page) - 100, "巡檢報告-月報表", font_big5_bold, 20);

    // 右上 報告產生日期
    std::string report_create_date = "報告產生日期: ";
    report_create_date.append(report_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 125, report_create_date, font_big5_light, 10);

    // 右上 報告資料區間
    std::string report_date_range = "報告資料區間: ";
    report_date_range.append(report_start_date);
    report_date_range.append(" 至 ");
    report_date_range.append(report_end_date);
    draw_big5_text(page, HPDF_Page_GetWidth(page) - 240, HPDF_Page_GetHeight (page) - 140, report_date_range, font_big5_light, 10);

    // Draw line
    // draw_line(page, 20, HPDF_Page_GetHeight (page) - 325, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight (page) - 325, 0.5);

    int font_size = 10;
    int rext_height = font_size * 2;
    int yoffset = HPDF_Page_GetHeight (page) - 260;
    int base_top_y = yoffset;
    int xoffset = 50;
    for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfData->headerlist.begin(); header != pdfData->headerlist.end(); header++)
    {
        if ( xoffset > HPDF_Page_GetWidth(page) - 50 )
        {
            xoffset = 50;
            base_top_y = yoffset - 20;
        }

        yoffset = base_top_y;

        int showWidth = draw_big5_text(page, xoffset + 5, yoffset, (*header)->header, font_big5_light, font_size);

        showWidth = (*header)->ShowWidth;
        // if ( header == pdfData->headerlist.begin())
        //     showWidth = 50;
        //
        // if (showWidth <= 20 )
        //     showWidth = 20;

        draw_rect(page, xoffset, yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);

        yoffset -= rext_height;
        // show row data
        //bool bFound = false;
        for (std::list<PDFTableRowDataStruct *>::iterator row = pdfData->rowdatalist.begin(); row != pdfData->rowdatalist.end(); row++)
        {
            PDFTableRowDataStruct * rowdata = (*row);

            bool bFound = false;
            for (std::list<PDFTableColDataStruct *>::iterator col = rowdata->coldatalist.begin(); col != rowdata->coldatalist.end(); col++)
            {
                PDFTableColDataStruct * colata = (*col);

                if ( colata->header.compare((*header)->header) == 0 )
                {
                    draw_big5_text(page, xoffset + 5, yoffset, colata->data, font_big5_light, font_size);

                    draw_rect(page, xoffset, yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);

                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                draw_big5_text(page, xoffset + 5, yoffset, "N/A", font_big5_light, font_size);

                draw_rect(page, xoffset, yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);
            }

            yoffset -= rext_height;
        }

        xoffset += showWidth;
        xoffset += 10;
    }

    xoffset = 50;
    draw_big5_text(page, xoffset + 5, yoffset, "※ --   表示該巡檢員未參與巡檢", font_big5_light, font_size);
    yoffset -= rext_height;
    draw_big5_text(page, xoffset + 5, yoffset, "※ N/A  表示未設定由該員巡檢", font_big5_light, font_size);

    // save the document to a file
    HPDF_SaveToFile (pdf, pdf_filename.c_str());

    // clean up
    HPDF_Free (pdf);

    vilslog::printf("PDFCreater::create_inspection_report() end.\n");
    vilslog::printf("==============================\n");
}

void PDFCreater::create_report_header(std::string pdf_filename, std::string report_title, std::string report_date, std::string report_start_date, std::string report_end_date)
{
    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::create_report_header() start.\n");

    m_pdf_filename = pdf_filename;
    m_pdf = HPDF_New (error_handler, NULL);
    if (!m_pdf)
    {
        vilslog::printf ("PDFCreater::create_report_header() error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (m_pdf);
        vilslog::printf ("PDFCreater::create_report_header() error: setjmp(env)\n");
        return;
    }

    // traditional Chinese Encodings
    HPDF_UseCNTEncodings(m_pdf);
    HPDF_UseCNTFonts(m_pdf);

    std::string appfilepath = APP_DIRECTORY;
    std::string font_big5_bold_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Bold.ttf";
    std::string font_big5_light_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Light.ttf";
    std::string font_big5_regular_filepath = appfilepath + "/pdf/fonts/TaipeiSansTCBeta-Regular.ttf";

    std::string font_big5_bold_name = HPDF_LoadTTFontFromFile(m_pdf, font_big5_bold_filepath.c_str(), HPDF_TRUE);
    m_font_big5_bold = HPDF_GetFont (m_pdf, font_big5_bold_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_light_name = HPDF_LoadTTFontFromFile(m_pdf, font_big5_light_filepath.c_str(), HPDF_TRUE);
    m_font_big5_light = HPDF_GetFont (m_pdf, font_big5_light_name.c_str(), "ETen-B5-H");// ETen-B5-V
    std::string font_big5_regular_name = HPDF_LoadTTFontFromFile(m_pdf, font_big5_regular_filepath.c_str(), HPDF_TRUE);
    m_font_big5_regular = HPDF_GetFont (m_pdf, font_big5_regular_name.c_str(), "ETen-B5-H");// ETen-B5-V

    // create default-font
    m_font_en_num = HPDF_GetFont (m_pdf, "Helvetica", NULL);

    HPDF_SetCompressionMode (m_pdf, HPDF_COMP_ALL);

    // add a new page object.
    m_page = HPDF_AddPage (m_pdf);

    HPDF_Page_SetWidth (m_page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (m_page, 842);

    m_dst = HPDF_Page_CreateDestination (m_page);
    HPDF_Destination_SetXYZ (m_dst, 0, HPDF_Page_GetHeight (m_page), 1);
    HPDF_SetOpenAction(m_pdf, m_dst);

    std::string image1_filepath = appfilepath + "/pdf/images/VILS_logo-2x.png";
    std::string image2_filepath = appfilepath + "/pdf/images/smims_logo.jpg";
    draw_image(m_pdf, PDF_IMAGE_PNG, image1_filepath.c_str(), 75, HPDF_Page_GetHeight (m_page) - 100, 50);
    draw_image(m_pdf, PDF_IMAGE_JPG, image2_filepath.c_str(), 50, HPDF_Page_GetHeight (m_page) - 150, 100);

    //
    // 右上 報告名稱
    //
    draw_big5_text(m_page, HPDF_Page_GetWidth(m_page) - 250, HPDF_Page_GetHeight (m_page) - 100, report_title, m_font_big5_bold, 20);

    // 右上 報告產生日期
    std::string report_create_date = "報告產生日期: ";
    report_create_date.append(report_date);
    draw_big5_text(m_page, HPDF_Page_GetWidth(m_page) - 240, HPDF_Page_GetHeight (m_page) - 125, report_create_date, m_font_big5_light, 10);

    // 右上 報告資料區間
    std::string report_date_range = "報告資料區間: ";
    report_date_range.append(report_start_date);
    report_date_range.append(" 至 ");
    report_date_range.append(report_end_date);
    draw_big5_text(m_page, HPDF_Page_GetWidth(m_page) - 240, HPDF_Page_GetHeight (m_page) - 140, report_date_range, m_font_big5_light, 10);

    m_yoffset = HPDF_Page_GetHeight (m_page) - 240;
}

void PDFCreater::create_report_userinfo(std::string report_type, std::string report_user)
{
    // 處理人員
    draw_big5_text(m_page, 50, HPDF_Page_GetHeight (m_page) - 180, report_type, m_font_big5_light, 12);

    // Draw line
    draw_line(m_page, 50, HPDF_Page_GetHeight (m_page) - 185, 200, HPDF_Page_GetHeight (m_page) - 185, 0.5);

    // 處理人員 名稱
    draw_big5_text(m_page, 50, HPDF_Page_GetHeight (m_page) - 200, report_user, m_font_big5_bold, 14);

    // 處理人員 部門
    //draw_big5_text(page, 50, HPDF_Page_GetHeight (page) - 260, report_user_orgName, font_big5_regular, 12);

    // 處理人員 職稱
    //draw_big5_text(m_page, 50, HPDF_Page_GetHeight (m_page) - 260, report_user_jobTitle, m_font_big5_regular, 12);

    // 事件列表
    //draw_big5_text(m_page, 50, HPDF_Page_GetHeight (m_page) - 320, "事件記錄", m_font_big5_light, 12);

    // Draw line
    draw_line(m_page, 50, HPDF_Page_GetHeight (m_page) - 205, 200, HPDF_Page_GetHeight (m_page) - 205, 1.0);
    //draw_line(m_page, 50, HPDF_Page_GetHeight (m_page) - 205, HPDF_Page_GetWidth(m_page) - 50, HPDF_Page_GetHeight (m_page) - 205, 0.5);

}

void PDFCreater::create_report_tableinfo(std::string info, int font_size)
{
    draw_big5_text(m_page, 50, m_yoffset, info, m_font_big5_light, font_size);

    m_yoffset -= (font_size * 2);
}

void PDFCreater::create_report_table(PDFTableDataStruct * pdfData)
{
    int font_size = 10;
    int rext_height = font_size * 2;
    int xoffset = 50;

    vilslog::printf("PDFCreater::create_report_table() pdfData->headerlist.size()[%d]\n", (int)pdfData->headerlist.size());

    // draw table header
    for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfData->headerlist.begin(); header != pdfData->headerlist.end(); header++)
    {
        int showWidth = (*header)->ShowWidth;
        // draw and fill/stroke a rectangle
        HPDF_Page_SetRGBFill(m_page, 0.75, 0.75, 0.75);
        HPDF_Page_Rectangle(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height);
        HPDF_Page_FillStroke(m_page);

        HPDF_Page_SetRGBFill(m_page, 0.0, 0.0, 0.0);

        draw_big5_text(m_page, xoffset + 5, m_yoffset, (*header)->header, m_font_big5_light, font_size);

        // vilslog::printf("PDFCreater::create_report_table() (*header)->header[%s] showWidth[%d]\n", (*header)->header.c_str(), showWidth);

        draw_rect(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);

        xoffset += showWidth;
        xoffset += 10;
    }

    m_yoffset -= rext_height;

    if(m_yoffset <= 20)
    {
        // add a new page object.
        m_page = HPDF_AddPage (m_pdf);
        m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
    }

    // vilslog::printf("PDFCreater::create_report_table() pdfData->rowdatalist.size()[%d]\n", (int)pdfData->rowdatalist.size());

    // show row data
    for (std::list<PDFTableRowDataStruct *>::iterator row = pdfData->rowdatalist.begin(); row != pdfData->rowdatalist.end(); row++)
    {
        PDFTableRowDataStruct * rowdata = (*row);

        xoffset = 50;
        // vilslog::printf("PDFCreater::create_report_table() xoffset[%d]\n", xoffset);

        int row_max_height = rext_height;
        for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfData->headerlist.begin(); header != pdfData->headerlist.end(); header++)
        {
            int showWidth = (*header)->ShowWidth;
            // if ( header == pdfData->headerlist.begin())
            //    showWidth = 120;

            // vilslog::printf("PDFCreater::create_report_table() rowdata->coldatalist.size()[%d]\n", (int)rowdata->coldatalist.size());

            for (std::list<PDFTableColDataStruct *>::iterator col = rowdata->coldatalist.begin(); col != rowdata->coldatalist.end(); col++)
            {
                PDFTableColDataStruct * colata = (*col);

                if ( colata->header.compare((*header)->header) == 0 )
                {
                    //draw_big5_text(m_page, xoffset + 5, m_yoffset, colata->data, m_font_big5_light, font_size);

                    // vilslog::printf("PDFCreater::create_report_table() header[%s] data[%s]\n", colata->header.c_str(), colata->data.c_str());

                    int draw_height = draw_big5_rect_text(m_page, xoffset + 5, m_yoffset, showWidth, colata->data, m_font_big5_light, font_size);
                    if ( row_max_height < draw_height )
                        row_max_height = draw_height;
                    // vilslog::printf("PDFCreater::create_report_table() rext_height[%d] draw_height[%d] row_max_height[%d]\n", rext_height, draw_height, row_max_height);

                    if ( draw_height < rext_height )
                        draw_height = rext_height;
                    draw_rect(m_page, xoffset, m_yoffset - (font_size/2) - (draw_height - rext_height), showWidth + 10, draw_height, 0.5);
                    //draw_rect(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);
                    break;
                }
            }
            xoffset += showWidth;
            xoffset += 10;
            // vilslog::printf("PDFCreater::create_report_table() xoffset[%d]\n", xoffset);
        }

        m_yoffset -= row_max_height;

        // vilslog::printf("PDFCreater::create_report_table() m_yoffset[%d]\n", m_yoffset);

        if(m_yoffset <= 20)
        {
            // add a new page object.
            m_page = HPDF_AddPage (m_pdf);
            m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
        }
    }


    m_yoffset -= 30;

    if(m_yoffset <= 20)
    {
        // add a new page object.
        m_page = HPDF_AddPage (m_pdf);
        m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
    }
}

void PDFCreater::create_report_table_old(PDFTableDataStruct * pdfData)
{
    int font_size = 10;
    int rext_height = font_size * 2;
    int base_top_y = m_yoffset;
    int xoffset = 50;
    for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfData->headerlist.begin(); header != pdfData->headerlist.end(); header++)
    {
        if ( xoffset > HPDF_Page_GetWidth(m_page) - 50 )
        {
            xoffset = 50;
            base_top_y = m_yoffset - 20;
        }

        m_yoffset = base_top_y;

        if(m_yoffset <= 20)
        {
            // add a new page object.
            m_page = HPDF_AddPage (m_pdf);
            m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
        }

        draw_big5_text(m_page, xoffset + 5, m_yoffset, (*header)->header, m_font_big5_light, font_size);
        int showWidth = (*header)->ShowWidth;

        // if ( header == pdfData->headerlist.begin())
        //    showWidth = 120;
        //
        // if (showWidth <= 20 )
        //     showWidth = 20;

        draw_rect(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);

        m_yoffset -= rext_height;

        if(m_yoffset <= 20)
        {
            // add a new page object.
            m_page = HPDF_AddPage (m_pdf);
            m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
        }

        // show row data
        //bool bFound = false;
        for (std::list<PDFTableRowDataStruct *>::iterator row = pdfData->rowdatalist.begin(); row != pdfData->rowdatalist.end(); row++)
        {
            PDFTableRowDataStruct * rowdata = (*row);

            bool bFound = false;
            for (std::list<PDFTableColDataStruct *>::iterator col = rowdata->coldatalist.begin(); col != rowdata->coldatalist.end(); col++)
            {
                PDFTableColDataStruct * colata = (*col);

                if ( colata->header.compare((*header)->header) == 0 )
                {
                    draw_big5_text(m_page, xoffset + 5, m_yoffset, colata->data, m_font_big5_light, font_size);

                    draw_rect(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);

                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                draw_big5_text(m_page, xoffset + 5, m_yoffset, "N/A", m_font_big5_light, font_size);

                draw_rect(m_page, xoffset, m_yoffset - (font_size/2), showWidth + 10, rext_height, 0.5);
            }

            m_yoffset -= rext_height;

            if(m_yoffset <= 20)
            {
                // add a new page object.
                m_page = HPDF_AddPage (m_pdf);
                m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
            }
        }

        xoffset += showWidth;
        xoffset += 10;
    }

    m_yoffset -= 30;

    if(m_yoffset <= 20)
    {
        // add a new page object.
        m_page = HPDF_AddPage (m_pdf);
        m_yoffset = HPDF_Page_GetHeight (m_page) - 50;
    }
}

void PDFCreater::generate_report()
{
    // save the document to a file
    HPDF_SaveToFile (m_pdf, m_pdf_filename.c_str());

    // clean up
    HPDF_Free (m_pdf);

    vilslog::printf("PDFCreater::generate_report() end.\n");
    vilslog::printf("==============================\n");
}

void PDFCreater::draw_line(HPDF_Page page, int startX, int startY, int endX, int endY, float lineWidth)
{
    HPDF_Page_SetLineWidth (page, lineWidth);
    HPDF_Page_MoveTo (page, startX, startY);
    HPDF_Page_LineTo (page, endX, endY);
    HPDF_Page_Stroke (page);
}

void PDFCreater::draw_rect(HPDF_Page page, int startX, int startY, int width, int height, float lineWidth)
{
    // 左下 -> 右下
    draw_line(page, startX, startY, startX + width, startY         , lineWidth);

    // 左下 -> 左上
    draw_line(page, startX, startY, startX        , startY + height, lineWidth);

    // 左上 -> 右上
    draw_line(page, startX, startY + height, startX + width, startY + height, lineWidth);

    // 右上 -> 右下
    draw_line(page, startX + width, startY + height, startX + width, startY, lineWidth);
}

int PDFCreater::draw_big5_text(HPDF_Page page, int posX, int posY, std::string in_string, HPDF_Font font, int font_size)
{
    HPDF_Page_BeginText (page);
    HPDF_Page_SetFontAndSize (page, font, font_size);
    HPDF_Page_MoveTextPos (page, posX, posY);
    std::string str_out_txt = utf_encode("BIG5", in_string.c_str());
    HPDF_Page_ShowText (page, str_out_txt.c_str());
    HPDF_Page_EndText (page);

    HPDF_UINT txtlen = (int)str_out_txt.length();
    HPDF_TextWidth textWidth = HPDF_Font_TextWidth(font, (const HPDF_BYTE *)str_out_txt.c_str(), txtlen);
    int textwidth = (textWidth.width * font_size) / 1000.0;
    return textwidth;
}

int PDFCreater::draw_big5_rect_text(HPDF_Page page, int posX, int posY, int width, std::string in_string, HPDF_Font font, int font_size)
{
    int lines = 1;
    HPDF_UINT len;

    std::string str_out_txt = utf_encode("BIG5", in_string.c_str());
    HPDF_UINT txtlen = (int)str_out_txt.length();
    HPDF_TextWidth textWidth = HPDF_Font_TextWidth(font, (const HPDF_BYTE *)str_out_txt.c_str(), txtlen);

    //HPDF_UINT xHeight = HPDF_Font_GetXHeight(font);
    //int baseline = (xHeight * font_size) / 1000.0;

    //vilslog::printf("PDFCreater::draw_big5_rect_text() txtlen[%d]\n",
        // in_string.length(), str_out_txt.length());

    int textwidth = (textWidth.width * font_size) / 1000.0;
    lines = trunc( ((float)textwidth / (float)width) + 0.99f);


    // vilslog::printf("PDFCreater::draw_big5_rect_text() textWidth.width[%d] textwidth[%d]\n",
    //     textWidth.width, textwidth);


    int draw_height = lines * (float)font_size;

    // font = 10
    // if ( lines == 2 )
    //     draw_height = 38;//ok
    // if ( lines == 3 )
    //     draw_height = 57;//
    // if ( lines >= 4 )
    //     draw_height = 76;//

    // font = 12
    // if ( lines == 2 )
    //     draw_height = 45;// ok
    // if ( lines == 3 )
    //     draw_height = 68;//ok
    // if ( lines >= 4 )
    //     draw_height = 90;//ok

    //font = 20
    // if ( lines == 2 )
    //     draw_height = 75;// ok
    // if ( lines == 3 )
    //     draw_height = 600;
    // if ( lines >= 4 )
    //     draw_height = 1000;

    HPDF_Page_BeginText (page);
    HPDF_Page_SetFontAndSize (page, font, font_size);
    HPDF_Page_MoveTextPos (page, posX, posY);
    HPDF_Page_SetTextLeading(page, 0);

    // vilslog::printf("PDFCreater::draw_big5_rect_text() width[%d] textwidth[%d] txtlen[%d] lines[%d] str_out_txt[%s]\n",
    //    width, textwidth, txtlen, lines, str_out_txt.c_str());

    //
    // test print to find available height
    //
    bool bFound = false;
    while(!bFound)
    {
        // vilslog::printf("PDFCreater::draw_big5_rect_text() draw_height[%d]\n", draw_height);


        //HPDF_Page_TextRect (page, posX, posY + font_size + 2*2, posX + width, posY - draw_height, str_out_txt.c_str(), HPDF_TALIGN_LEFT, &len);
        HPDF_Page_TextRect (page, 5000, 5000, 5000 + width, 5000 - draw_height, str_out_txt.c_str(), HPDF_TALIGN_LEFT, &len);
        // vilslog::printf("PDFCreater::draw_big5_rect_text() len[%d]\n", len);

        if ( len >= txtlen )
        {
            bFound = true;
            // vilslog::printf("PDFCreater::draw_big5_rect_text() bFound[%d]\n", bFound);
        }
        else
        {
            draw_height += 10;
        }

        if (draw_height > 300)
            break;
    }

    //real print
    float top_y = posY + ((float)font_size*1.4f);// move up
    HPDF_Page_TextRect (page, posX, top_y, posX + width, top_y - draw_height, str_out_txt.c_str(), HPDF_TALIGN_LEFT, &len);
    //vilslog::printf("PDFCreater::draw_big5_rect_text() len[%d]\n", len);


    HPDF_Page_EndText (page);
    return draw_height;
}

int PDFCreater::draw_events(HPDF_Page page, int startX, int startY, std::string event_time, std::string resident, std::string event_process_time, std::string event_comment, HPDF_Font font, int font_size)
{
    // 日期時間
    draw_big5_text(page, startX, startY, event_time.c_str(), font, font_size);

    // 求助住戶
    draw_big5_text(page, startX + 100, startY, resident.c_str(), font, font_size);

    // 開始處理時間
    draw_big5_text(page, startX + 210, startY, event_process_time.c_str(), font, font_size);

    // 事件說明
    int draw_height = draw_big5_rect_text(page, startX + 300, startY,  HPDF_Page_GetWidth(page) - 350 - 50, event_comment.c_str(), font, font_size);

    return draw_height;
}

int PDFCreater::draw_inout_events(HPDF_Page page, int startX, int startY, std::string in_datetime, std::string out_datetime, std::string area_name, std::string total_time, HPDF_Font font, int font_size)
{
    // 進入 日期時間
    draw_big5_text(page, startX, startY, in_datetime.c_str(), font, font_size);

    // 離開 日期時間
    draw_big5_text(page, startX + 100, startY, out_datetime.c_str(), font, font_size);

    // 區域名稱
    draw_big5_text(page, startX + 210, startY, area_name.c_str(), font, font_size);

    // 停留時間
    //int draw_height = draw_big5_rect_text(page, startX + 300, startY,  HPDF_Page_GetWidth(page) - 350 - 50, total_time.c_str(), font, font_size);
    int draw_height = draw_big5_rect_text(page, startX + 390, startY,  HPDF_Page_GetWidth(page) - 440 - 50, total_time.c_str(), font, font_size);

    return draw_height;
}

int PDFCreater::draw_firedevent_events(HPDF_Page page, int startX, int startY, std::string event_datetime, std::string user, std::string action, std::string area_name, std::string in_datetime, std::string out_datetime, std::string total_time, HPDF_Font font, int font_size)
{
    // 日期時間
    draw_big5_text(page, startX, startY, event_datetime.c_str(), font, font_size);

    // user
    draw_big5_text(page, startX + 100, startY, user.c_str(), font, font_size);

    // action
    draw_big5_text(page, startX + 175, startY, action.c_str(), font, font_size);

    // 區域名稱
    draw_big5_text(page, startX + 225, startY, area_name.c_str(), font, font_size);

    // 進入 日期時間
    // draw_big5_text(page, startX + 350, startY, in_datetime.c_str(), font, font_size);

    // 離開 日期時間
    draw_big5_text(page, startX + 300, startY, out_datetime.c_str(), font, font_size);

    // 停留時間
    //int draw_height = draw_big5_rect_text(page, startX + 300, startY,  HPDF_Page_GetWidth(page) - 350 - 50, total_time.c_str(), font, font_size);
    int draw_height = draw_big5_rect_text(page, startX + 400, startY,  HPDF_Page_GetWidth(page) - 440 - 50, total_time.c_str(), font, font_size);

    return draw_height;
}

void PDFCreater::draw_image(HPDF_Doc pdf, PDF_IMGAE_TYPE img_type, const char * filename, float x, float y, float draw_width)
{
    //const char* FILE_SEPARATOR = "/";

    char filename1[255];

    HPDF_Page page = HPDF_GetCurrentPage (pdf);
    HPDF_Image image;

    //strcpy(filename1, "images");
    //strcpy(filename1, "pdf");
    //strcat(filename1, FILE_SEPARATOR);
    strcpy(filename1, filename);

    if (img_type == PDF_IMAGE_PNG)
        image = HPDF_LoadPngImageFromFile (pdf, filename1);
    else
        image = HPDF_LoadJpegImageFromFile (pdf, filename1);

    int image_width = HPDF_Image_GetWidth (image);
    int image_height = HPDF_Image_GetHeight (image);
    float ratio = (float)image_height / (float)image_width;

    float show_width = draw_width;
    float show_height = show_width * ratio;

    // Draw image to the canvas.
    HPDF_Page_DrawImage (page, image, x, y, show_width, show_height);

}


void PDFCreater::show_description(HPDF_Page page, float x, float y, const char * text)
{
    char buf[255];

    HPDF_Page_MoveTo (page, x, y - 10);
    HPDF_Page_LineTo (page, x, y + 10);
    HPDF_Page_MoveTo (page, x - 10, y);
    HPDF_Page_LineTo (page, x + 10, y);
    HPDF_Page_Stroke (page);

    HPDF_Page_SetFontAndSize (page, HPDF_Page_GetCurrentFont (page), 8);
    HPDF_Page_SetRGBFill (page, 0, 0, 0);

    HPDF_Page_BeginText (page);

    snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);

    HPDF_Page_MoveTextPos (page, x - HPDF_Page_TextWidth (page, buf) - 5,
            y - 10);
    HPDF_Page_ShowText (page, buf);
    HPDF_Page_EndText (page);

    HPDF_Page_BeginText (page);
    HPDF_Page_MoveTextPos (page, x - 20, y - 25);
    HPDF_Page_ShowText (page, text);
    HPDF_Page_EndText (page);
}

// const char * PDFCreater::utf_encode(const char *code, const char *in_buf, size_t in_len)
std::string PDFCreater::utf_encode(const char *code, std::string in_string)
{
    iconv_t cd = iconv_open(code, "UTF-8");
    //iconv_t cd = iconv_open(code, "ISO_8859-1");
    //iconv_t cd = iconv_open("GBK", "UTF-8");
    //iconv_t cd = iconv_open("BIG5", "UTF-8");
    //iconv_t cd = iconv_open("BIG5-HKSCS", "UTF-8");
    //iconv_t cd = iconv_open("CP950", "UTF-8");

    if (cd == (iconv_t)-1)
    {
        vilslog::printf("PDFCreater::utf_encode() ERROR !!! cd == (iconv_t)-1\n");
        return "";
    }
    size_t in_len = in_string.length();

    size_t sz = in_len * in_len;
    char * tmp_str = (char *)malloc(sz);

    // 不要将原始的指针传进去，那样会改变原始指针的

    size_t inlen = in_len;
    size_t outlen = sz;
   // char *in = (char *)in_buf;
    char *in = (char *)in_string.c_str();
    char *out = tmp_str;

    if (tmp_str == NULL)
    {
        iconv_close(cd);
        vilslog::printf("PDFCreater::utf_encode() ERROR !!! tmp_str == NULL\n");
        return "";
    }

    memset(tmp_str, 0, sz);
    if (iconv(cd, &in, &inlen, &out, &outlen) == (size_t)-1)
    {
        vilslog::printf("PDFCreater::utf_encode() ERROR !!! in[%c] in_len[%d] in[%c] outlen[%d]\n", in, (int)inlen, out, (int)outlen);
        //vilslog::printf("PDFCreater::utf_encode() 3.1 tmp_str[%s]\n", tmp_str);

        delete [] tmp_str;
        iconv_close(cd);
        return "";
    }

    std::string outString = tmp_str;

    delete [] tmp_str;
    iconv_close(cd);

    return outString;
}

void PDFCreater::test_pdf_creater()
{
    HPDF_Doc  pdf;
    HPDF_Font font;
    //HPDF_Font detail_font;
    HPDF_Page page;
    char fname[256];
    HPDF_Destination dst;

    vilslog::printf("\n==============================\n");
    vilslog::printf("PDFCreater::test_pdf_creater() start.\n");


    //const char * out_txt = "简体中文";
    //const char * c_out_txt = utf_2_gbk(out_txt, strlen(out_txt));

    // const char * out_txt = "北瀚科技緊急事件報告";
    // const char * c_out_txt = utf_encode("GBK", out_txt, strlen(out_txt));

    strcpy (fname, "vils_test_");
    strcat (fname, ".pdf");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf)
    {
        vilslog::printf ("error: cannot create PdfDoc object\n");
        return;
    }

    // error-handler
    if (setjmp(env))
    {
        HPDF_Free (pdf);
        return;
    }

    //=======================================
    HPDF_UseCNTEncodings(pdf);
    HPDF_UseCNTFonts(pdf);
    // //--- Use Built-in Chinese font ---
    //detail_font = HPDF_GetFont (pdf, "MingLiU","ETen-B5-H");
    //detail_font = HPDF_GetFont (pdf, "MingLiU,Bold","ETen-B5-H");
    //detail_font = HPDF_GetFont (pdf, "MingLiU,Italic","ETen-B5-H");
    //detail_font = HPDF_GetFont (pdf, "MingLiU,BoldItalic","ETen-B5-H");
    //detail_font = HPDF_GetFont (pdf, "MingLiU","ETHK-B5-H"); // X
    //detail_font = HPDF_GetFont (pdf, "MingLiU","B5pc-H"); // X
    //detail_font = HPDF_GetFont (pdf, "MingLiU","ETenms-B5-H"); // X
    //Identity-H、Identity-V、B5pc-H、B5pc-V、ETen-B5-H、ETen-B5-V、ETenms-B5-H、ETenms-B5-V、ETHK-B5-H、ETHK-B5-V
    //=======================================
    //HPDF_UseCNSFonts(pdf);
    //HPDF_STATUS retStatus = HPDF_UseCNSEncodings(pdf);
    // detail_font = HPDF_GetFont (pdf, "SimSun", "GBK-EUC-H");// GB-EUC-H GB-EUC-V GBK-EUC-H GBK-EUC-V
    //detail_font = HPDF_GetFont (pdf, "SimHei", "GBK-EUC-H");// GB-EUC-H GB-EUC-V GBK-EUC-H GBK-EUC-V
    //=======================================
    // vilslog::printf("PDFCreater::test_pdf_creater() HPDF_GetFont ok!\n");
    //
    // if ( retStatus == HPDF_OK )
    // {
    //     vilslog::printf("PDFCreater::test_pdf_creater() HPDF_UseCNSEncodings  HPDF_OK\n");
    // }
    // else
    // {
    //     vilslog::printf("PDFCreater::test_pdf_creater() HPDF_UseCNSEncodings Not OK!!!! \n");
    // }

    //=====================================
    //sprintf(font_name, "%s", HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Bold.ttf", HPDF_TRUE));
    //std::string font_name = HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Bold.ttf", HPDF_TRUE);
    //std::string font_name = HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Light.ttf", HPDF_TRUE);
    std::string font_name = HPDF_LoadTTFontFromFile(pdf, "pdf/fonts/TaipeiSansTCBeta-Regular.ttf", HPDF_TRUE);

    //detail_font = HPDF_GetFont(pdf, HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Bold.ttf", HPDF_TRUE), "StandardEncoding");
    //HPDF_Font font_test = HPDF_GetFont(pdf, HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Bold.ttf", HPDF_TRUE), "UTF-8");
    //HPDF_Font font_test = HPDF_GetFont(pdf, HPDF_LoadTTFontFromFile(pdf, "TaipeiSansTCBeta-Bold.ttf", HPDF_TRUE), "StandardEncoding");

    vilslog::printf("PDFCreater::test_pdf_creater() font_name[%s] \n", font_name.c_str());

    //HPDF_Font font_test = HPDF_GetFont (pdf, font_name.c_str(), "ETHK-B5-H");
    HPDF_Font font_test_B5 = HPDF_GetFont (pdf, font_name.c_str(), "ETen-B5-H");

    //HPDF_Page_SetFontAndSize (page, detail_font, 12);
    //=====================================


    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    // create default-font
    font = HPDF_GetFont (pdf, "Helvetica", NULL);



    //vilslog::printf("HPDF_Font_GetEncodingName (detail_font) = %s\n", HPDF_Font_GetEncodingName (detail_font));

    // add a new page object.
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, 595); // 72px/inch:595×842, 150px/inch:1240×1754, 2480×3508
    HPDF_Page_SetHeight (page, 842);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    draw_image(pdf, PDF_IMAGE_JPG, "pdf/images/N32.jpg", (595 - 300)/2, HPDF_Page_GetHeight (page) - 250, 300);

    HPDF_Page_BeginText (page);
    //HPDF_Page_SetFontAndSize (page, detail_font, 20);
    HPDF_Page_SetFontAndSize (page, font_test_B5, 20);
    HPDF_Page_MoveTextPos (page, (595 - 250)/2, HPDF_Page_GetHeight (page) - 250);
    //std::string str_out_txt = utf_encode("CP950", "北瀚科技緊急事件報告");
    //std::string str_out_txt = utf_encode("BIG5-HKSCS", "北瀚科技緊急事件報告");
    std::string str_out_txt = utf_encode("BIG5", "北瀚科技緊急事件報告");
    //std::string str_out_txt = utf_encode("GBK", "北瀚科技緊急事件報告");
    HPDF_Page_ShowText (page, str_out_txt.c_str());



    //=======================================
    // test GBK
    HPDF_UseCNSFonts(pdf);
    HPDF_UseCNSEncodings(pdf);
    std::string font_name_GBK = HPDF_LoadTTFontFromFile(pdf, "pdf/fonts/TaipeiSansTCBeta-Regular.ttf", HPDF_TRUE);
    HPDF_Font font_test_GBK = HPDF_GetFont (pdf, font_name_GBK.c_str(), "GBK-EUC-H");

    HPDF_Page_SetFontAndSize (page, font_test_GBK, 20);
    HPDF_Page_MoveTextPos (page, 0, -20);
    std::string str_out1_txt = utf_encode("GBK", "简体中文");
    HPDF_Page_ShowText (page, str_out1_txt.c_str());
    //=======================================



    HPDF_Page_SetFontAndSize (page, font, 20);
    HPDF_Page_MoveTextPos (page, 0, -20);
    //std::string str_ABC_out_txt = utf_encode("GBK", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    HPDF_Page_ShowText (page, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    HPDF_Page_MoveTextPos (page, 0, -20);
    HPDF_Page_ShowText (page, "abcdefghijklmnopqrstuvwxyz");

    HPDF_Page_MoveTextPos (page, 0, -20);
    HPDF_Page_ShowText (page, "1234567890");

    HPDF_Page_EndText (page);

    //====================
    // Draw line
    HPDF_Page_SetLineWidth (page, 0.5);
    HPDF_Page_MoveTo (page, 10, HPDF_Page_GetHeight (page) - 550);
    HPDF_Page_LineTo (page, HPDF_Page_GetWidth (page) - 10, HPDF_Page_GetHeight (page) - 550);
    HPDF_Page_Stroke (page);
    //====================


    // save the document to a file
    HPDF_SaveToFile (pdf, fname);

    // clean up
    HPDF_Free (pdf);

    vilslog::printf("PDFCreater::test_pdf_creater() end.\n");
    vilslog::printf("\n==============================\n");
}

 // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
std::string PDFCreater::getUserType(int type)
{
    std::string Usertypename = "";
    switch(type)
    {
        default:
        case 0:
            Usertypename = "人員訪客";
        break;
        case 1:
            Usertypename = "巡檢員";
        break;
        case 2:
            Usertypename = "設備";
        break;
        case 3:
            Usertypename = "VIP貴賓";
        break;
        case 4:
            Usertypename = "工作人員";
        break;
        case 5:
            Usertypename = "醫師";
        break;
        case 6:
            Usertypename = "護理師";
        break;
    }
    return Usertypename;
}

std::string PDFCreater::getLocatorType(int type)
{
    std::string Usertypename = "";
    switch(type)
    {
        default:
        case 0:
            Usertypename = "緊急按鈕";
        break;
        case 1:
            Usertypename = "呼喚鈕";
        break;
    }
    return Usertypename;
}
