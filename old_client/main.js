let querying = false;
function getQueryResult() {

    if (querying) return;

    querying = true;

    let start_query = new Date();

    document.getElementById("queryResult").textContent =
        "Querying model, please await response.";

    const prompt = document.getElementById("prompt").value;
    
    fetch(`/query?prompt=${prompt}`)
        .then((response) => response.json())
        .then((data) => {
            querying = false;
            let end_query = new Date();
            let time_elapsed =
                (end_query.getTime() - start_query.getTime()) / 1000;

            document.getElementById("queryResult").innerHTML =
                data.data + "<br> time elapsed: " + time_elapsed + "s";
        })
        .catch((error) => console.error("Error:", error));
}

function setContext() {
    if (querying) return;
    querying = true;
    document.getElementById("contextResult").textContent = "Loading...";
    const ctx = document.getElementById("ctx").value;
    fetch(`/set_context?ctx=${ctx}`)
        .then((response) => response.json())
        .then((data) => {
            querying = false;
            document.getElementById("contextResult").textContent =
                JSON.stringify(data);
        })
        .catch((error) => console.error("Error:", error));
}

function setParameter() {
    if (querying) return;
    querying = true;
    document.getElementById("parameterResult").textContent = "Loading...";
    const key = document.getElementById("key").value;
    const value = document.getElementById("value").value;
    fetch(`/set_parameter?key=${key}&value=${value}`)
        .then((response) => response.json())
        .then((data) => {
            querying = false;
            document.getElementById("parameterResult").textContent =
                JSON.stringify(data);
        })
        .catch((error) => console.error("Error:", error));
}

function setBaseModel() {
    if (querying) return;
    querying = true;
    document.getElementById("baseModelResult").textContent = "Loading...";
    const baseModel = document.getElementById("baseModel").value;
    fetch(`/set_base_model?model=${baseModel}`)
        .then((response) => response.json())
        .then((data) => {
            querying = false;
            document.getElementById("baseModelResult").textContent =
                JSON.stringify(data);
        })
        .catch((error) => console.error("Error:", error));
}

let base_model_options_cache = [];
function getBaseModelOptions() {
    if (document.getElementById("baseModelOptions").innerHTML !== "") {
        document.getElementById("baseModelOptions").innerHTML = "";
        return;
    }
    if (base_model_options_cache.length > 0)
        render_base_model_options(base_model_options_cache);
    fetch(`/base_model_options`)
        .then((response) => response.json())
        .then((data) => {
            base_model_options_cache = data;
            render_base_model_options(data);
        })
        .catch((error) => console.error("Error:", error));
}

function render_base_model_options(data) {
    const table = document.createElement("table");
    const headers = Object.keys(data[0]);
    const headerRow = document.createElement("tr");
    headers.forEach((header) => {
        const th = document.createElement("th");
        th.textContent = header;
        headerRow.appendChild(th);
    });
    table.appendChild(headerRow);
    data.forEach((entry) => {
        const row = document.createElement("tr");
        headers.forEach((header) => {
            const td = document.createElement("td");
            td.textContent = entry[header];
            row.appendChild(td);
        });
        table.appendChild(row);
    });

    document.getElementById("baseModelOptions").innerHTML = "";
    document.getElementById("baseModelOptions").appendChild(table);
}

let model_parameter_options_cache = [];
function getModelParameterOptions() {
    if (document.getElementById("modelParameterOptions").innerHTML != "") {
        document.getElementById("modelParameterOptions").innerHTML = "";
        return;
    }
    if (model_parameter_options_cache.length > 0)
        render_parameter_options(model_parameter_options_cache);

    fetch(`/model_parameter_options`)
        .then((response) => response.json())
        .then((data) => {
            model_parameter_options_cache = data;
            render_parameter_options(model_parameter_options_cache);
        })
        .catch((error) => console.error("Error:", error));
}

function render_parameter_options(data) {
    const table = document.createElement("table");
    const headers = ["param_name", "type", "desc"];
    const headerRow = document.createElement("tr");
    headers.forEach((header) => {
        const th = document.createElement("th");
        th.textContent = header;
        headerRow.appendChild(th);
    });
    table.appendChild(headerRow);

    data.forEach((entry) => {
        const row = document.createElement("tr");
        headers.forEach((header) => {
            const td = document.createElement("td");
            td.textContent = entry[header];
            row.appendChild(td);
        });
        table.appendChild(row);
    });

    document.getElementById("modelParameterOptions").innerHTML = "";
    document.getElementById("modelParameterOptions").appendChild(table);
}
