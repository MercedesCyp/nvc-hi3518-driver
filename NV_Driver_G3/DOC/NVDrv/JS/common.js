/**
 * Created by Mojies on 7/30/2015.
 */

function CreateTableFormArra(id,arr){
    var eid = document.getElementById(id);
    var tbl_o1 = arr.length;
    var tbl_o2 = arr[0].length;
    for( var i=0; i<tbl_o1;i++ ){
        var tTr = document.createElement("tr");
        if( i%2 == 1 ){
            tTr.setAttribute("bgcolor","#D2EAF1")
        }
        for( var j=0;j<tbl_o2;j++ ){
            var ttd = document.createElement("td");
            ttd.innerHTML = arr[i][j];
            tTr.appendChild(ttd);
        }
        eid.appendChild(tTr);
    }
    eid.setAttribute("rules","all");
    eid.setAttribute("align","center");
    eid.setAttribute("width","61%");

    for( var i=0; i<eid.childNodes.length;i++ ) {
        if (eid.childNodes[i].tagName == "TBODY"){
            eid.childNodes[i].setAttribute("bgcolor","#4BACC6");
            break;
        }
    }
}

function CreateBackToTop(){
    var v_but = document.createElement("button");

    v_but.innerHTML="To Top";
    v_but.setAttribute("style","position:fixed;width:60px;height:30px;right:60px;bottom:60px;");
    v_but.setAttribute("onclick","jf_RunToTop()");
    document.body.appendChild(v_but);
}

function jf_RunToTop(){
    window.scroll(0,0);
}

function jf_ShowCurTime(){
    var ttb = document.createElement("div");
    ttb.setAttribute("style","float: right;");
    ttb.setAttribute("id","realTimeShow");
    var tBasicHead = document.getElementById("id_pagehead");
    tBasicHead.appendChild(ttb);
    window.setTimeout(UpdateTime(),1000);
}


function UpdateTime(){
    var year,month,day,week,hour,minute,second;
    var curDate;
    var standWeekStr = ["星期日","星期一","星期二","星期三","星期四","星期五","星期六"];
    var ttb = document.getElementById("realTimeShow");
    curDate=new Date();
    year = curDate.getYear() + 1900;
    month = curDate.getMonth();
    day = curDate.getDate();
    week = standWeekStr[curDate.getDay()];
    hour = curDate.getHours();
    minute = curDate.getMinutes();
    second = curDate.getSeconds();
    ttb.innerHTML=year+"年"+month+"月"+day+"日 "+week+" "+hour+"："+minute+"："+second;

    window.setTimeout("UpdateTime();",1000);
    //window.setTimeout(UpdateTime(ttb),1000);
}