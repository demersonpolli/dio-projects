import React from "react";
import "./ChatMessage.css";

import { AvatarAI } from "./AvatarAI.mjs";
import { AvatarUser } from "./AvatarUser.mjs";

export const ChatMessage = ({user, message}) => {
    return(
        <div className={`chat-message ${user === "AI" ? "ai" : "user"}`}>
            <div className="chat-message-center">
                <div className={`avatar ${user === "AI" ? "avatar-ai": "avatar-user"}`}>
                    {user === "AI" ? <AvatarAI/> : <AvatarUser/>}
                </div>
                <div className="message">{message}</div>
            </div>
        </div>
    );
}