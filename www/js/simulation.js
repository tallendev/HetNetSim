/*
 * File: simulation.js
 * Last Edit: 7/21/2014
 * Author: Matthew Leeds
 * Author: Tyler Allen
 * Purpose: This script is the backend for the HetNet Simulation in index.html.
 * It provides interactivity and communicates with the server to solve 
 * linear programming problems. The main() function sets up the canvas and default
 * values for variables. The changeMode() function is the controller that 
 * calls appropriate functions to implement the modes. Pretty much every other
 * function is a helper for those two.
 */

// document level variables
var backgroundColor;
var canvas;
var borderPath;
var w;
var h;
var x;
var y;
var xAxis;
var yAxis;

// simulation level variables
var simData;
var buttonFuncs;
var colorArray;
var color;
var shape;
var mode;
var deviceID;
var networkID;
var currentColor;
var filterColor;
var maxRates;
var instructions;
var textBox;
var alpha; // weight of aggregate throughput in the optimization
var beta;  // weight of fairness in the optimization

// constants
var TRANSITION_DURATION = 500; // # of ms elements take to fade in or out
var OPACITY_LEVEL = 0.5;       // full opacity level
var DEVICES_COLOR = "darkgray";  
var BORDER_WIDTH = 5;         
var BORDER_COLOR = "gray";    
var GRIDLINE_GRANULARITY = 20; // determines fine/coarse gridlines


/*
 * The main function sets up the canvas, its border and axes,
 * and the default choices in the side menu.
 */
function main()
{
    w = $("body").width() - 260; // account for the width of the sidebar
    h = $("body").height();

    canvas = d3.select("#simulation")
               .append("svg")
               .attr("width", w)
               .attr("height", h);    

    canvas.append("g").attr("id", "layer1"); // to control ordering of elements
    canvas.append("g").attr("id", "layer2");

    textBox = canvas.select("#layer2")
                    .append("text")
                    .attr("x", w / 2)
                    .attr("y", 40)
                    .attr("fill", "darkgray")
                    .attr("font-size", "medium")
                    .attr("font-weight", "bold")
                    .attr("text-anchor", "middle")
                    .text("");

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    maxRates = {"red": 0, "green": 0, "blue": 0};
    instructions = {"Add"     : "Click anywhere to add a network or device.",
                    "Resize"  : "Click and drag on any network to resize it.",
                    "Move"    : "Click and drag on any network to move it.",
                    "Delete"  : "Click on any network or device to delete it.",
                    "Filter"  : "Filter networks and devices using the switches.",
                    "Optimize": "Adjust the slider to change the weights of optimization parameters."
                   }
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

    x = d3.scale.linear().range([0, w]);
    y = d3.scale.linear().range([h, 0]); 
    xAxis = d3.svg.axis().scale(x).orient("bottom")
                                  .ticks(w / GRIDLINE_GRANULARITY)
                                  .tickSize(-h,0,0)
                                  .tickFormat("");
    yAxis = d3.svg.axis().scale(y).orient("left")
                                  .ticks(h / GRIDLINE_GRANULARITY)
                                  .tickSize(-w,0,0)
                                  .tickFormat("");
    drawAxes();

    borderPath = canvas.select("#layer1")
                       .append("rect")
                       .attr("x", 0)
                       .attr("y", 0)
                       .attr("width", w)
                       .attr("height", h)
                       .style("stroke", BORDER_COLOR)
                       .style("fill", "none")
                       .style("stroke-width", BORDER_WIDTH);

}

/*
 * distance uses the pythagorean theorem to calculate the magnitude of the 
 * distance between two points.
 */
function distance(x1, y1, x2, y2)
{
    return Math.sqrt(
           Math.pow(Math.abs(x2 - x1), 2) +
           Math.pow(Math.abs(y2 - y1), 2));
}

/*
 * The drawAxes function appends the x and y axes to the canvas
 * and translates the x axis so it starts in the bottom left instead 
 * of the top left.
 */
function drawAxes() {
    canvas.select("#layer1").append("g")
                            .attr("class", "grid")
                            .attr("transform", "translate(0," + h + ")")
                            .call(xAxis);
    canvas.select("#layer1").append("g")
                            .attr("class", "grid")
                            .call(yAxis);
}


/*
 * gatherData analyzes the simulation to collect the normalized distances
 * between every device and access point, and stores these in simData. 
 */ 
function gatherData()
{
    simData = {};
    for (var i = 1; i <= deviceID; i++)
    {
        var currentDeviceID = "#device" + i;
        var deviceInfo = {};
        var xforms = $(currentDeviceID).prop("transform").baseVal;
        for (var j = 1; j <= networkID; j++)
        {
            var currentNetworkID = "#network" + j;
            var distanceFromPoint = distance(xforms.getItem(0).matrix.e, 
                                             xforms.getItem(0).matrix.f,
                                             $(currentNetworkID).prop("cx").baseVal.value,
                                             $(currentNetworkID).prop("cy").baseVal.value);
            var radius = $(currentNetworkID).prop("r").baseVal.value;
            if (distanceFromPoint < radius)
                deviceInfo[currentNetworkID] = distanceFromPoint / radius;
            else
                deviceInfo[currentNetworkID] = -1;
        }
        simData[currentDeviceID] = deviceInfo;
    }
    console.log(simData);
}


/*
 * The genCircle function can be called to generate new circle elements.
 * It takes in the svg, location, radius, and color.
 */    
function genCircle(svg, cx, cy, radius, newColor)
{
    var theColor = d3.rgb(newColor);
    networkID++;
    if (maxRates[newColor] == 0)
    {
        maxRates[newColor] = prompt("Please enter the maximum possible " 
                                  + "bandwidth rate for this network type "
                                  + "in bits/s:");
    }
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


/* 
 * The genTriangle function generates triangles using paths.
 * It ignores the color parameter which is there for consistency with
 * genCircle so either can be called with one statement. The triangle has to
 * be translated to the appropriate place because symbols start at the origin.
 */
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

/*
 * changeText transitions the opacity of the text box and changes its text
 * based on the current mode.
 */
function changeText()
{
    textBox.style("opacity", 0);
    textBox.text(instructions[mode])
           .transition()
           .style("opacity", 1)
           .duration(TRANSITION_DURATION);
    textBox.transition()
           .style("opacity", 0)
           .duration(TRANSITION_DURATION)
           .delay(3000)
           .text("");
}

/*
 * modeAdd enables the add functionality on the canvas and
 * shows the shape and color options to the user.
 */
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


/*
 * modeResize enables resizing functionality by calling the
 * resize function when a circle is dragged.
 */
function modeResize()
{
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
}


/*
 * modeMove allows users to move shapes by changing the drag behavior. 
 * For circles, the origin is set as the location of the circle at
 * the start of the drag so x and y coordinates are relative to that, which
 * makes the motion smoother.
 */
function modeMove()
{
    d3.selectAll("circle").call(d3.behavior.drag()
                                           .origin(circleOrigin)
                                           .on("drag", dragCircle));
    d3.selectAll("path").call(d3.behavior.drag()
                                         .on("drag", dragPath));
}


/*
 * modeDelete lets users delete elements making use of d3's remove function.
 */
function modeDelete()
{
    d3.selectAll("circle").on("click", function()
                              {
                                  d3.select(this).remove();
                              });
    d3.selectAll("path").on("click", function()
                              {
                                  d3.select(this).remove();
                              });
}


/*
 * modeFilter shows the switchs that let the user change the parameters
 * of the filter and calls updateFilter, which makes the canvas reflect those
 * selections.
 */
function modeFilter()
{
    $('#filters').show();
    updateFilter();
}


function modeOptimize()
{
    $('#optimization').show();
    gatherData();
}

/*
 * changeMode is called whenever the user clicks on a mode, and once when
 * the page loads. It removes all behaviors on the canvas and enables 
 * appropriate ones using the functions above.
 */
function changeMode()
{
    mode = $('input[name=mode]:checked').val();
    
    $('#shape').hide();
    $('#color').hide();
    $('#filters').hide();
    $('#optimization').hide();
    showAll(); // shows all elements (circles and triangles)
    removeBehaviors(); // disables click and drag behaviors

    switch (mode)
    {
        case "Add":
        {
            modeAdd();
            break;
        }
        case "Resize":
        {
            modeResize();
            break;
        }
        case "Move":
        {
            modeMove();
            break;
        }
        case "Delete":
        {
            modeDelete();
            break;
        }
        case "Filter":
        {
            modeFilter();
            break;
        }
        case "Optimize":
        {
            modeOptimize();
            break;
        }
    }

    changeText();
}


/*
 * changeShape is called when the user selects a shape. It updates the 
 * global shape variable and decides whether to show the color options.
 */
function changeShape()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle") 
        $('#color').show();
    else
        $('#color').hide();
}


/*
 * changeColor is called when the user selects a color, and updates the 
 * global color variable.
 */
function changeColor()
{
    color = $('input[name=color]:checked').val();
}


/*
 * resize uses the pythagorean theorem to calculate the cursor's distance 
 * from the center of the circle, and changes the radius appropriately.
 */
function resize()
{
    var thisCircle = d3.select(this);
    var mouse = d3.mouse(this);
    var newRadius = distance(thisCircle.attr("cx"),
                             thisCircle.attr("cy"),
                             mouse[0],
                             mouse[1]);
    thisCircle.attr("r", newRadius);
}


/*
 * Drag behaviors can only give x and y coordinates relative to an origin if 
 * that origin object has x and y attributes, so this function is necessary 
 * to bridge that gap.
 */
function circleOrigin()
{
    return { x: d3.select(this).attr("cx"),
             y: d3.select(this).attr("cy") };
}


/* 
 * dragCircle changes the selected circle's position based on the drag event's
 * coordinates, which are offsets based on its original position.
 */
function dragCircle()
{
    d3.select(this).attr("cx", d3.event.x)
                   .attr("cy", d3.event.y);
}


/*
 * dragPath is used to move paths (triangles in this case), using the 
 * transform attribute.
 */
function dragPath()
{
    d3.select(this).attr("transform", function()
    {
        return "translate(" + d3.event.x + "," + d3.event.y + ")";
    });
}


/*
 * updateFilter makes all elements visible, and if the mode is set to filter,
 * it selectively hides appropriate elements. To make it smooth, the opacity
 * is transitioned to 0 over a short duration, and then the visibility is
 * set to hidden, which ensures that click or drag events cannot occur.
 * To hide the correct colors and stay "scalable", it iterates over the
 * colorArray, and assumes a switch has been defined for each in the HTML
 * after the network and devices switches.
 */
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
            var switchID = '#switch' + (i+3).toString().replace(/^[0]+/g,"");
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


/*
 * showAll smoothly transitions all elements to full opacity/color 
 * to make them visible.
 */
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


/*
 * removeBehaviors removes all click and drag behaviors from the canvas
 * and elements to ensure that none persist into the wrong mode.
 */
function removeBehaviors()
{
    canvas.on("click", null);
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("path").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("circle").on("click", null);
    d3.selectAll("path").on("click", null);
}


/*
 * updateParams updates the variable used to weight fairness and aggregate
 * throughput, alpha and beta, and reflects those values in the sidebar.
 */
function updateParams()
{
   beta = parseFloat($('#param1').val()).toFixed(2);
   alpha = (1 - Number(beta)).toFixed(2);
   $('#fairnessVal').html("Fairness Weight: " + beta);
   $('#throughputVal').html("Throughput Weight: " + alpha);
}


/*
 * optimize uses the values of alpha and beta, and the data in simData to 
 * form a linear programming problem to optimize the system. Then it calls 
 * the LPSolver written in C++ using a shared library.
 */
function optimize()
{
    //TODO: implement
    /*$.ajax({
        type     : 'POST',
        url      : 'process.php',
        data     : {'problem' : $('#problem').val()},
        dataType : 'json',
        success  : function(data) {
            if (data.success) {
                $('#result').append("<p>" + data.answer + "</p>");
            } else {
                console.log("POST in function 'optimize()' failed.");
            }
        }
    });*/
}

// When the page loads, open the sidebar.
$(document).ready(function() {
    $('#simple-menu').sidr(side="left");
    jQuery.sidr(method="open")
});

main();

