using Djinni.TestSuite;
using NUnit.Framework;

namespace Djinni.Testing.Unit
{
    [TestFixture]
    public class TokenTest
    {
        private class CsToken : UserToken
        {
            public override string Whoami()
            {
                return "C#";
            }
        }

        [Test]
        public void TestTokens()
        {
            UserToken token = new CsToken();
            Assert.AreSame(token, TestHelpers.TokenId(token));
        }
}
}