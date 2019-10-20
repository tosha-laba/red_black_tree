function inputKeysAreCorrect() {
    let values = InputKeysInput.value.split(" ").filter((v, i, arr) => v != "");

    if (values.length == 0) {
        return false;
    }

    return values.every((v, i, arr) => isNumber(v));
}

function deleteKeyIsCorrect() {
    let value = DeleteKeyInput.value;

    return isNumber(value);
}

function informInputAboutError(input, isCorrect) {
    if (isCorrect) {
        input.style.backgroundColor = "#ffffff";
    } else {
        input.style.backgroundColor = "rgba(255, 0, 0, 0.35)";
    }
}

function informInputAboutGoodOperation(input, isGood) {
    if (isGood) {
        input.style.backgroundColor = "rgba(0, 255, 0, 0.35)";
    } else {
        input.style.backgroundColor = "rgba(0, 0, 255, 0.35)";
    }
}

function isNumber(value) {
    if (value == null || value == undefined) {
        return false;
    }

    if (typeof (value) != "string") {
        return false;
    }

    if (value.indexOf('-') > 0) {
        return false;
    } else if (value.indexOf('-') == 0) {
        value = value.substring(1);
    }

    return !Number.isNaN(Number.parseInt(value)) &&
        value.split("").every((v, i, arr) => v.charCodeAt(0) >= 48 && v.charCodeAt(0) <= 57);
}

function convertInputKeysToArray() {
    return InputKeysInput.value.split(" ").filter((v, i, arr) => v != "").map((v, i, arr) => Number.parseInt(v));
}

function convertDeleteKeyToInt() {
    return Number.parseInt(DeleteKeyInput.value);
}

function clearAll() {
    FirstTree.innerHTML = "";
    SecondTree.innerHTML = "";
    InputKeysInput.style.backgroundColor = "#fff";
    DeleteKeyInput.style.backgroundColor = "#fff";
}