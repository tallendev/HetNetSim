/*
 *
 */

var backgroundColor;

var canvas;
var borderPath;
var info;

var buttonFuncs;
var colorArray;
var color;
var shape;
var mode;
var deviceID;
var networkID;
var switchID;
var currentColor;
var filterColor;

var TRANSITION_DURATION = 500;
var OPACITY_LEVEL = 0.5;
var DEVICES_COLOR = "gray";
var BORDER_WIDTH = 5;
var BORDER_COLOR = "gray";

function main()
{
    var w = $("body").width() * .7;
    var h = $("body").height();

    canvas = d3.select("#simulation")
               .append("svg")
               .attr("width", w)
               .attr("height", h);    

    borderPath = canvas.append("rect")
                           .attr("x", 0)
                           .attr("y", 0)
                           .attr("width", w)
                           .attr("height", h)
                           .style("stroke", BORDER_COLOR)
                           .style("fill", "none")
                           .style("stroke-width", BORDER_WIDTH);

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    shape = "circle";
    $('input[value="Add"]').prop('checked', true);
    $('input[value="circle"]').prop('checked', true);
    $('#color1').prop('checked', true);
    changeMode();

    deviceID = 0;
    networkID = 0;

    backgroundColor = $("body").css("background-color");

    canvas.on("mousemove", function()
    {
        $('#info').empty();
        $('#info').append("<p> Location: (" + d3.mouse(this)[0].toFixed() +
                          ", " + d3.mouse(this)[1].toFixed() + ")</p>");
    });

}


function genCircle(svg, cx, cy, radius, newColor)
{
    var theColor = d3.rgb(newColor);
    networkID++;
    svg.append("circle")
          .attr("id", "network" + networkID)
          .attr("cx", cx)
          .attr("cy", cy)
          .attr("r", radius)
          .attr("stroke", theColor.darker(.9))
          .attr("stroke-width", radius / 10)
          .attr("opacity", OPACITY_LEVEL)
          .style("fill", theColor);
}

function genTriangle(svg, xCoord, yCoord, size, newColor)
{
    deviceID++;
    svg.append("path")
          .attr("id", "device" + deviceID)
          .attr("class", "point")
          .style("fill", DEVICES_COLOR)
          .attr("d", d3.svg.symbol().type("triangle-up"))
          .attr("size", size)
          .attr("transform", function() {
              return "translate(" + xCoord + "," + yCoord + ")"; 
              });
}

function modeAdd()
{
    $('#shape').show();
    $('#color').show();
    canvas.on("click", function()
    {
        drawFunc = buttonFuncs[shape];
        if (drawFunc != null)
        {
            drawFunc(canvas, d3.mouse(this)[0], 
                             d3.mouse(this)[1], 50, color);
        }
    });
}

function modeResize()
{
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
}

function modeMove()
{
    d3.selectAll("circle").call(d3.behavior.drag()
                                           .origin(circleOrigin)
                                           .on("drag", dragCircle));
    d3.selectAll("path").call(d3.behavior.drag()
                                         .on("drag", dragPath));
}

function modeDelete()
{
    d3.selectAll("circle").on("click", deleteItem);
    d3.selectAll("path").on("click", deleteItem);
}

function modeFilter()
{
    $('#filters').show();
    updateFilter();
}

function changeMode()
{
    mode = $('input[name=mode]:checked').val();

    $('#shape').hide();
    $('#color').hide();
    $('#filters').hide();
    showAll(); // shows all elements (circles and triangles)
    removeBehaviors(); // disables click and drag behaviors

    switch (mode)
    {
        case "Add":
            modeAdd();
            break;
        case "Resize":
            modeResize();
            break;
        case "Move":
            modeMove();
            break;
        case "Delete":
            modeDelete();
            break;
        case "Filter":
            modeFilter();
            break;
    }
}

function changeShape()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle") 
        $('#color').show();
    else
        $('#color').hide();
}

function changeColor()
{
    color = $('input[name=color]:checked').val();
    console.log(color);
}

function resize()
{
    var thisCircle = d3.select(this);
    var mouse = d3.mouse(this);
    var newRadius = Math.sqrt( // pythagorean theorem 
                    Math.pow(Math.abs(thisCircle.attr("cx") - mouse[0]), 2) +
                    Math.pow(Math.abs(thisCircle.attr("cy") - mouse[1]), 2));
    thisCircle.attr("r", newRadius);
}

function circleOrigin()
{
    return { x: d3.select(this).attr("cx"),
             y: d3.select(this).attr("cy") };
}

function dragCircle()
{
    d3.select(this).attr("cx", d3.event.x)
                   .attr("cy", d3.event.y);
}

function dragPath()
{
    d3.select(this).attr("transform", function()
    {
        return "translate(" + d3.event.x + "," + d3.event.y + ")";
    });
}

function deleteItem()
{
    d3.select(this).remove();
}

function updateFilter()
{
    console.log("updateFilter()");
    showAll();
    if (mode === "Filter")
    {
        if (!$('#switch1').prop('checked'))
        {
            d3.selectAll("circle").transition()
                                  .style("opacity", 0)
                                  .duration(TRANSITION_DURATION)
                                  .transition()
                                  .style("visibility", "hidden")
                                  .duration(0);
        }
        if (!$('#switch2').prop('checked'))
        {
            d3.selectAll("path").transition()
                                .style("fill", backgroundColor)
                                .duration(TRANSITION_DURATION);
        }
        for (var i = 0; i < colorArray.length; i++)
        {
            switchID = '#switch' + (i+3).toString().replace(/^[0]+/g,"");
            if (!$(switchID).prop('checked'))
            {
                console.log("on " + switchID);
                d3.selectAll("circle").filter( function()
                {
                    currentColor = d3.rgb(d3.select(this).style("fill"));
                    filterColor = d3.rgb($(switchID).attr("value")
                                                        .toLowerCase());
                    if (currentColor.toString() == filterColor.toString())
                        return d3.select(this);
                }).transition()
                  .style("opacity", 0)
                  .duration(TRANSITION_DURATION);
            }
        }                                                               
    }
}

function showAll()
{
    d3.selectAll("circle").transition()
                          .style("opacity", OPACITY_LEVEL)
                          .duration(TRANSITION_DURATION)
                          .style("visibility", "visible");
    d3.selectAll("path").transition()
                        .style("fill", DEVICES_COLOR)
                        .duration(TRANSITION_DURATION);
}

function removeBehaviors()
{
    canvas.on("click", null);
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("path").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("circle").on("click", null);
    d3.selectAll("path").on("click", null);
}

$(document).ready(function() {
    $('#simple-menu').sidr(side="left");
    jQuery.sidr(method="open")

    $('form').submit(function(event)
    {
        $('#result').empty();
        $.ajax({
            type     : 'POST',
            url      : 'process.php',
            data     : {'problem' : $('#problem').val()},
            dataType : 'json',
            success  : function(data) {
                if (data.success) {
                    $('#result').append("<p>" + data.answer + "</p>");
                } else {
                    console.log("failure");
                }
            }
        });
        event.preventDefault();
    });
});

main();

