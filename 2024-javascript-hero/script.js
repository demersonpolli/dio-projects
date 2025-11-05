let numButtonClicks = 0;
function checkPoints() {
    const heroi = document.getElementById("name").value;
    const points = parseInt(document.getElementById("xp").value);
    numButtonClicks = numButtonClicks + 1;
    
    var level = "";
    if (points <= 1000)
        level = "Ferro";
    else if (points <= 2000)
        level = "Bronze";
    else if (points <= 5000)
        level = "Prata";
    else if (points <= 7000)
        level = "Ouro";
    else if (points <= 8000)
        level = "Platina";
    else if (points <= 9000)
        level = "Ascendente";
    else if (points <= 10000)
        level = "Imortal";
    else
        level = "Radiante";
    
    document.getElementById("message").textContent =
        `O herói de nome **${heroi}** está no nivel de **${level}**.`;
}

