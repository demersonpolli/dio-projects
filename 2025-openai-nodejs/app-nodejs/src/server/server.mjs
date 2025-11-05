import OpenAI from 'openai';


export const makeRequest = async ({prompt}) => {
    let key = process.env.OPENAI_API_KEY;
    const openai = new OpenAI({apiKey: key, dangerouslyAllowBrowser: true});

    try {
        const response = await openai.chat.completions.create({
            model: "gpt-3.5-turbo",
            messages: [
                {
                    role: "system",
                    content: "You are a helpful travel assistant. You will do a motorcycle travel plan. " +
                    "Please indicate the destination, the starting point, and an estimate of  days to travel. " +
                    "Keep the distance traveled in one day below 600 kilometers."
                },
                {
                    role: "user",
                    content: prompt
                }
            ],
            temperature: 0.7,
            max_tokens: 2048,
            top_p: 1.0,
            frequency_penalty: 0.0,
            presence_penalty: 0.0
        });

        return {
            success: true,
            data: response.choices[0].message.content
        };
    }
    catch (error) {
        return {
            success: false,
            error: error.response? error.response.data : "An error occurred."
        }
    }
};