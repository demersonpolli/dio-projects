"use client";

import Image from "next/image";
import styles from "./page.module.css";

import { useState } from "react";
import { SideMenu } from "@/components/SideMenu.mjs";
import { ChatMessage } from "@/components/ChatMessage.mjs";
import { makeRequest } from "@/server/server.mjs";

export default function Home() {
    const[input, setInput] = useState("");
    const[chatLog, setChatLog] = useState([
        {
            "user": "AI",
            "message": "Hello! I am a motorcycle travel assistant. How can I help you today?"
        }
    ]);

    async function handleSubmit(evt) {
        evt.preventDefault();

        let response = await makeRequest({prompt: input});
        response = <p>{response.data}</p>;
        
        setChatLog([
            ...chatLog,
            {
                "user": "User",
                "message": input
            },
            {
                "user": "AI",
                "message": response
            }
        ]);
        setInput("");
    }

    return (
        <div className={styles.page}>
            <aside className={styles.sidebar}>
                <SideMenu/>
            </aside>

            <main className={styles.main}>
                <Image className={styles.logo} src="/next.svg" alt="Next.js logo" width={180} height={38} priority/>
                <section className={styles.chatbox}>
                    <div className={styles.chatlog}>
                        {chatLog.map((message, index) => (
                            <div key={index} className={styles.entry}>
                                <ChatMessage key={index} user={message.user} message={message.message}/>
                            </div>
                        ))}
                    </div>
                    <div className={styles.chatinput}>
                        <form onSubmit={handleSubmit}>
                            <input className={styles.chattext} rows="1" type="text" value={input} onChange={(evt) => setInput(evt.target.value)} placeholder="Type your message here"/>
                        </form>
                    </div>
                </section>
            </main>
        </div>
    );
}
