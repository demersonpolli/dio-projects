const state = {
    score: {
        playerScore: 0,
        computerScore: 0,
        scoreBox: document.getElementById('score-points'),
    },
    cardSprites: {
        avatar: document.getElementById('card-image'),
        name: document.getElementById('card-name'),
        type: document.getElementById('card-type'),
    },
    fieldCards: {
        player: document.getElementById('player-field-card'),
        computer: document.getElementById('computer-field-card'),
    },
    playerSides: {
        player: "player-cards",
        playerBox: document.querySelector("#player-cards"),
        computer: "computer-cards",
        computerBox: document.querySelector("#computer-cards"),
    },
    actions: {
        button: document.getElementById('next-duel'),
    },
};


const pathImages = "./assets/icons";
const cardData = [
    {
        id: 0,
        name: "Blue Eyes White Dragon",
        type: "Paper",
        img: `${pathImages}/dragon.png`,
        winsOf: [1],
        loseOf: [2]
    },
    {
        id: 1,
        name: "Dark Magician",
        type: "Rock",
        img: `${pathImages}/magician.png`,
        winsOf: [2],
        loseOf: [0]
    },
    {
        id: 2,
        name: "Exodia",
        type: "Scissors",
        img: `${pathImages}/exodia.png`,
        winsOf: [0],
        loseOf: [1]
    },
]


async function getRandomCardId() {
    const randomIndex = Math.floor(Math.random() * cardData.length);
    return cardData[randomIndex].id;
}


async function createCardImage(cardId, fieldSide) {
    const cardImage = document.createElement("img");
    cardImage.setAttribute("height", "100px");
    cardImage.setAttribute("src", "./assets/icons/card-back.png");
    cardImage.setAttribute("data-id", cardId);
    cardImage.classList.add("card");

    if (fieldSide === state.playerSides.player) {
        cardImage.addEventListener("mouseover", () => {
            drawSelectedCard(cardId);
        });

        cardImage.addEventListener("click", () => {
            setCardsField(cardImage.getAttribute("data-id"));
        });
    }

    return cardImage;
}


async function checkDuelResult(playerCardId, computerCardId) {
    let duelResult = "draw";
    let playerCard = cardData[playerCardId];

    if (playerCard.winsOf.includes(computerCardId)) {
        duelResult = "win";
        state.score.playerScore++;
    }

    if (playerCard.loseOf.includes(computerCardId)) {
        duelResult = "lose";
        state.score.computerScore++;
    }

    await playAudio(duelResult);

    return duelResult;
}


async function showCardImages(showCards) {
    if (showCards) {
        state.fieldCards.computer.style.display = "block";
        state.fieldCards.player.style.display = "block";
    }
    else {
        state.fieldCards.computer.style.display = "none";
        state.fieldCards.player.style.display = "none";
    }
}

async function drawCardsInfield(playerCardId, computerCardId) {
    state.fieldCards.player.src = cardData[playerCardId].img;
    state.fieldCards.computer.src = cardData[computerCardId].img;
}

async function hideCardDetails() {
    state.cardSprites.avatar.src = "";
    state.cardSprites.name.innerText = "";
    state.cardSprites.type.innerText = "";
}


async function drawButton(text) {
    state.actions.button.innerText = text.toUpperCase();
    state.actions.button.style.display = "block";
}


async function updateScore() {
    state.score.scoreBox.innerText = `Win: ${state.score.playerScore} | Lose: ${state.score.computerScore}`;
}

async function removeAllCardsImages() {
    let { computerBox, playerBox } = state.playerSides;
    let imgElements = computerBox.querySelectorAll("img");
    imgElements.forEach((img) => img.remove());

    imgElements = playerBox.querySelectorAll("img");
    imgElements.forEach((img) => img.remove());
}


async function setCardsField(cardId) {
    await removeAllCardsImages();

    let computerCardId = await getRandomCardId();

    await showCardImages(true);

    await hideCardDetails();

    await drawCardsInfield(cardId, computerCardId);

    let duelResult = await checkDuelResult(cardId, computerCardId);

    await updateScore();
    await drawButton(duelResult);
}


async function drawSelectedCard(index) {
    state.cardSprites.avatar.src = cardData[index].img;
    state.cardSprites.name.innerText = cardData[index].name;
    state.cardSprites.type.innerText = "Attribute: " + cardData[index].type;
}


async function drawCards(quantity, fieldSide) {
    for (let i = 0; i < quantity; i++) {
        const cardId = await getRandomCardId();
        const cardImage = await createCardImage(cardId, fieldSide);

        document.getElementById(fieldSide).appendChild(cardImage);
    }    
}


async function resetDuel() {
    state.cardSprites.avatar.src = "";
    state.actions.button.style.display = "none";

    state.fieldCards.player.style.display = "none";
    state.fieldCards.computer.style.display = "none";

    init();
}


async function playAudio(status) {
    if (status === "draw") return;

    const audio = new Audio(`./assets/audios/${status}.wav`);
    audio.play();
}


function init() {
    showCardImages(false);

    drawCards(5, state.playerSides.player);
    drawCards(5, state.playerSides.computer);

    const bgm = document.getElementById("bgm").play();
}

init();
