/*
 *
 */

var w;
var h;
var border;
var borderColor;

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

function main()
{
    w = 960;
    h = 640;
    border = 5;
    borderColor = "gray";

    canvas = d3.select("#simulation")
               .append("svg")
               .attr("width", w)
               .attr("height", h);

   borderPath = canvas.append("rect")
                           .attr("x", 0)
                           .attr("y", 0)
                           .attr("width", w)
                           .attr("height", h)
                           .style("stroke", borderColor)
                           .style("fill", "none")
                           .style("stroke-width", border);

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}//, 
                   //'resize' : setResizable};
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    shape = "circle";
    $('input[value="add"]').prop('checked', true);
    modeChange();
    $('input[value="circle"]').prop('checked', true);
    $('input[value="red"]').prop('checked', true);

    deviceID = 0;
    networkID = 0;


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
          //value for opacity subject to change. we may want to use light colors
          //exclusively in the final version, the dark colors probably aren't that
          //much easier to see.
          .attr("opacity", .5)
          .style("fill", theColor)
}

function genTriangle(svg, xCoord, yCoord, size, newColor)
{
    deviceID++;
    svg.append("path")
          .attr("id", "device" + deviceID)
          .attr("class", "point")
          .style("fill", "gray")
          .attr("d", d3.svg.symbol().type("triangle-up"))
          .attr("size", size)
          .attr("transform", function() {
              return "translate(" + xCoord + "," + yCoord + ")"; 
              });
}

function modeAdd(on)
{
    if (on)
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
    else
    {
        $('#shape').hide();
        $('#color').hide();
        canvas.on("click", null);
    }
}

function modeResize(on)
{
    if (on)
        d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
    else
        d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
}

function modeMove(on)
{
    if (on)
    {
        d3.selectAll("circle").call(d3.behavior.drag()
                                               .origin(circleOrigin)
                                               .on("drag", dragCircle));
        d3.selectAll("path").call(d3.behavior.drag()
                                             .on("drag", dragPath));
    }
    else
    {
        d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
        d3.selectAll("path").call(d3.behavior.drag().on("drag", null));
    }
}

function modeDelete(on)
{
    if (on)
    {
        d3.selectAll("circle").on("click", deleteItem);
        d3.selectAll("path").on("click", deleteItem);
    }
    else
    {
        d3.selectAll("circle").on("click", null);
        d3.selectAll("path").on("click", null);
    }
}

function modeChange()
{
    mode = $('input[name=mode]:checked').val();

    if (mode === "add") 
        modeAdd(true);
    else 
        modeAdd(false);

    if (mode === "resize") 
        modeResize(true);
    else if (mode === "move")
        modeMove(true);
    else 
        modeMove(false);

    if (mode === "delete")
        modeDelete(true);
    else
        modeDelete(false);

    /*if (mode === "filter")
        modeFilter(true);
    else
        modeFilter(false);*/
}

function shapeChange()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle")
        $('#color').show();
    else
        $('#color').hide();
}

function colorChange()
{
    color = $('input[name=color]:checked').val();
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

$(document).ready(function() {
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
